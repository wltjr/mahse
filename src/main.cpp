#include <argp.h>
#include <mpi.h>

#include <iostream>
#include <random>

#include "agent.hpp"

const char *argp_program_version = "Version 0.1";
const char *argp_program_bug_address = "@unf.edu";

// command line arguments
struct args
{
    int agents;
    int dim;
    int tasks;
};

// help menu
static struct argp_option options[] = {
    {0,0,0,0,"Optional arguments:",1},
    {"agents",'a',"5",0," Number of agents, min 5 ",2},
    {"dimension",'d',"500",0," Grid dimensions, e.g. 500 ",2},
    {"tasks",'t',"2",0," Number of tasks, min 2 ",2},
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

    struct args *args = (struct args*)state->input;

    switch(key) {
        case 'a':
            args->agents = arg ? atoi (arg) : 5;
            break;
        case 'd':
            args->agents = arg ? atoi (arg) : 500;
            break;
        case 't':
            args->tasks = arg ? atoi (arg) : 2;
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
    char buffer[BUFFER_SIZE];
    int rank;
    int size;
    int position;
    double timer;
    struct args args;
    std::vector<Task> tasks;
    Agent agent;
    Point coords;

    // default arguments
    args.agents = 5;
    args.dim = 500;
    args.tasks = 2;

    // parse command line options
    argp_parse (&argp, argc, argv, 0, 0, &args);

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

        std::uniform_real_distribution<> urd2(5, 10);
        std::uniform_real_distribution<> urd3(2, 3);

        coords.x = 0;
        coords.y = 0;

        tasks.emplace_back(0, coords, 0, 0);
    
        for(int i = 0; i < args.tasks; i++)
        {
            int reward;
            int modifier;

            coords.x = urd1(gen);
            coords.y = urd1(gen);
            reward = urd2(gen);
            modifier = urd3(gen);

            tasks.emplace_back(i+1, coords, reward, modifier);
        }

        std::cout << "Tasks:" << std::endl;
        for (auto& task : tasks) {
            std::cout << "Id: " << task.get_id()
                      << ", Coords: (" << task.get_coords().x << ',' << task.get_coords().y
                      << "), Reward: " << task.get_reward()
                      << ", Modifier: " << task.get_modifier() << std::endl;
        }
    }
    std::cout << std::endl;

    // Add null task to count
    args.tasks++;

    // Broadcast the size of the vector
    MPI_Bcast(&args.tasks, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Root packs the data
    position = 0;
    if (rank == 0)
    {
        for (auto& task : tasks)
            task.pack(buffer, BUFFER_SIZE, position, MPI_COMM_WORLD);
    }

    // Broadcast the packed data length and data
    MPI_Bcast(&position, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&buffer, position, MPI_INT, 0, MPI_COMM_WORLD);

    // All processes except root unpack the data
    if (rank !=0 )
    {
        tasks.resize(args.tasks);
        position = 0;
        for (int i = 0; i < args.tasks; i++)
            tasks[i].unpack(buffer, BUFFER_SIZE, position, MPI_COMM_WORLD);
    }

    coords.x = urd1(gen);
    coords.y = urd1(gen);

    agent = Agent(rank + 1, size, coords, tasks, Agent::submodular);
    agent.decision();

    if (rank == 0)
    {
        timer = MPI_Wtime() - timer;
        std::cout << "Elapsed time: " << timer << "s" << std::endl;
    }

    MPI_Finalize();

    return 0;
}
