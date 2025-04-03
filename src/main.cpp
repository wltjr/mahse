#include <argp.h>
#include <mpi.h>

#include <iostream>

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
    int rank;
    int size;
    double timer;
    struct args args;

    // default arguments
    args.agents = 2;
    args.dim = 500;
    args.tasks = 5;

    // parse command line options
    argp_parse (&argp, argc, argv, 0, 0, &args);

    // Initialize MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
        // get MPI wall time
        timer = MPI_Wtime();

    std::cout << "I serve no purpose!" << std::endl;

    if (rank == 0)
    {
        timer = MPI_Wtime() - timer;
        std::cout << "Elapsed time: " << timer << "s" << std::endl;
    }

    MPI_Finalize();
}
