#ifndef AGENT_HPP
#define AGENT_HPP

#include <tuple>
#include <vector>

#include "task.hpp"

class Agent
{
    public:
        enum rewards {
            peaked,
            submodular
        };

        /**
         * @brief Construct a new Agent object, empty/unused
         */
        Agent();

        /**
         * @brief Construct a new Agent object
         *
         * @param id the task id
         * @param agents the number of agents
         * @param coords the task coordinates (x,y)
         * @param tasks vector of tasks
         * @param reward the reward heuristic to use
         */
        Agent(int id, int agents, Point coords, std::vector<Task> &tasks, rewards reward);

        /**
         * @brief Decentralized decision-making algorithm
         */
        void decision();

        /**
         * @brief Destroy the Agent object, empty/unused
         */
        virtual ~Agent();

    private:
        int id;
        int agents;
        Point coords;
        bool satisfied;
        std::vector<std::vector<int>> partitions;  // Π
        std::vector<Task> tasks; // T
        int iterations; // r
        double seed; // s [0,1] uniform-random variable
        int reward;
        int task; // the task index number, e.g. the j in (t_j, S_j)

        /**
         * @brief Euclidean distance between task and agent
         *
         * @param task the task index number, e.g. the j in (t_j, S_j)
         */
        int distance(int task);

        /**
         * @brief Pack a class instance into contiguous memory for MPI
         *
         * @param buffer pointer to char buffer used to store instance
         * @param size buffer size, in bytes (non-negative integer)
         * @param position current position in buffer, in bytes (integer)
         * @param comm 	communicator for packed message (handle)
         */
        void pack_msg(char *buffer, int size, int &position, MPI_Comm comm);

        /**
         * @brief Unpack a class instance stored contiguous memory from MPI
         *
         * @param buffer pointer to char buffer used to store instance
         * @param size buffer size, in bytes (non-negative integer)
         * @param position current position in buffer, in bytes (integer)
         * @param comm 	communicator for packed message (handle)
         */
        std::vector<std::tuple<int, double, std::vector<std::vector<int>>>>
            unpack_msgs(char *buffer, int size, int &position, MPI_Comm comm);

        /**
         * @brief Utility function
         *
         * @param task the task index number, e.g. the j in (t_j, S_j)
         */
        int utility(int task);
};
#endif
