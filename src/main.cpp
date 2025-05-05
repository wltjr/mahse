#include <argp.h>
#include <mpi.h>

#include <format>
#include <iostream>
#include <random>

#include "grape.hpp"

const char *argp_program_version = "Version 0.1";
const char *argp_program_bug_address = "@unf.edu";

// command line arguments
struct args
{
    int dim;
    int modifier;
    int reward;
    int tasks;
    int utility;
};

// help menu
static struct argp_option options[] = {
    {0,0,0,0,"Optional arguments:",1},
    {"dimension",'d',"1000",0," Grid dimensions, e.g. 1000 ",1},
    {"modifier",'m',"2",0," Reward modifier integer, random if unset ",1},
    {"reward",'r',"10",0," Reward value integer, random if unset ",1},
    {"tasks",'t',"2",0," Number of tasks, min 2 ",1},
    {"utility",'u',"0",0," Utility reward type 0 for peak and 1 for submodular reward, e.g. 1",1},
    {0,0,0,0,"GNU Options:", 2},
    {0,0,0,0,0,0}
};

/**
 * Parse command line options
 *
 * @key the integer key provided by the current option to the option parser.
 * @arg the name of an argument associated with this option
 * @state points to a struct argp_state
 *
 * @return ARGP_ERR_UNKNOWN for any key value not recognize
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {

    auto args = (struct args*)state->input;

    switch(key) {
        case 'd':
            args->dim = arg ? atoi (arg) : 1000;
            break;
        case 'm':
            args->modifier = arg ? atoi (arg) : 0;
            break;
        case 'r':
            args->reward = arg ? atoi (arg) : 0;
            break;
        case 't':
            args->tasks = arg ? atoi (arg) : 2;
            break;
        case 'u':
            args->utility = arg ? atoi (arg) : Grape::submodular;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return(0);
}

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
static struct argp argp	 =  { options, parse_opt };

int main(int argc, char* argv[])
{
    const int BUFFER_SIZE = 1024;
    std::vector<char> buffer(BUFFER_SIZE);
    int rank;
    int size;
    int position;
    double timer;
    struct args args;
    std::vector<Task> tasks;
    Grape agent;
    Point coords;

    // default arguments
    args.dim = 1000;
    args.modifier = 0;
    args.reward = 0;
    args.tasks = 2;
    args.utility = Grape::submodular;

    // parse command line options
    argp_parse (&argp, argc, argv, 0, 0, &args);

    // min tasks check
    if(args.tasks < 2)
        args.tasks = 2;

    // random functions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> urd1(0, args.dim);

    // Initialize MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        // get MPI wall time
        timer = MPI_Wtime();

        std::uniform_real_distribution<> urd2(2, 3);
        std::uniform_real_distribution<> urd3(args.dim, args.dim + args.dim);

        coords.x = 0;
        coords.y = 0;

        std::cout << "Reward Heuristic: ";
        switch(args.utility)
        {
            case Grape::peaked:
                std::cout << "Peaked-Reward";
                break;
            default:
                std::cout << "Submodular";
                break;
        }
        std::cout << std::endl;

        tasks.emplace_back(0, coords, 0, 0);
    
        for(int i = 0; i < args.tasks; i++)
        {
            coords.x = urd1(gen);
            coords.y = urd1(gen);

            // random reward & modifier from paper if not specified
            if(!args.reward && !args.modifier)
            {
                switch(args.utility)
                {
                    case Grape::peaked:
                        args.reward = urd3(gen) * size / args.tasks;
                        args.modifier = size / args.tasks; // n^d_j rounded (r^max_j / ∑_{∀t_k∈T*} r^max_k) * n_a
                        break;
                    default:
                        args.modifier = urd2(gen);
                        args.reward = urd3(gen) *
                                1 / (std::log(size / args.tasks + 1) / std::log(args.modifier));
                        break;
                }
            }

            tasks.emplace_back(i+1, coords, args.reward, args.modifier);
        }

        // Display tasks information
        std::cout << args.tasks << " Tasks:" << std::endl;
        for (auto& task : tasks) 
        {
            std::cout <<
                std::format("Id: {}, Coords: ({},{}), Reward: {}, Modifier: {}",
                            task.get_id(), task.get_coords().x, task.get_coords().y,
                            task.get_reward(), task.get_modifier()) << std::endl;
        }
        std::cout << std::endl;
    }

    // Add null task to count
    args.tasks++;

    // Broadcast the size of the vector
    MPI_Bcast(&args.tasks, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Root packs the data
    position = 0;
    if (rank == 0)
    {
        for (auto& task : tasks)
            task.pack(&buffer[0], BUFFER_SIZE, position, MPI_COMM_WORLD);
    }

    // Broadcast the packed data length and data
    MPI_Bcast(&position, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&buffer[0], position, MPI_INT, 0, MPI_COMM_WORLD);

    // All processes except root unpack the data
    if (rank !=0 )
    {
        tasks.resize(args.tasks);
        position = 0;
        for (int i = 0; i < args.tasks; i++)
            tasks[i].unpack(&buffer[0], BUFFER_SIZE, position, MPI_COMM_WORLD);
    }

    coords.x = urd1(gen);
    coords.y = urd1(gen);

    agent = Grape(rank + 1, size, coords, tasks, static_cast<Grape::rewards>(args.utility));

    // Display agent information
    if (rank == 0)
        std::cout << size << " Agents:" << std::endl;
    std::cout << std::format("Id: {}, Coords: ({},{})",
                             rank + 1, coords.x, coords.y) << std::endl;
    if (rank == 0)
        std::cout << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);

    // start algorithm
    agent.decision();

    if (rank == 0)
    {
        timer = MPI_Wtime() - timer;
        std::cout << "Elapsed time: " << timer << "s" << std::endl;
    }

    MPI_Finalize();

    return 0;
}
