#ifndef GRAPE_HPP
#define GRAPE_HPP

#include <tuple>
#include <vector>

#include "agent.hpp"
#include "task.hpp"

class Grape: public Agent
{
    public:
        enum rewards {
            peaked,
            submodular
        };

        /**
         * @brief Construct a new Agent object, empty/unused
         */
        Grape();

        /**
         * @brief Construct a new Agent object
         *
         * @param id the task id
         * @param agents the number of agents
         * @param coords the task coordinates (x,y)
         * @param tasks vector of tasks
         * @param reward the reward heuristic to use
         */
        Grape(int id, int agents, Point coords, std::vector<Task> &tasks, rewards reward);

        /**
         * @brief Decentralized decision-making algorithm
         */
        void decision() override;

        /**
         * @brief Destroy the Agent object, empty/unused
         */
        virtual ~Grape();

    private:
        bool satisfied;
        std::vector<std::vector<int>> partitions;  // Π
        int iterations; // r
        double seed; // s [0,1] uniform-random variable

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
        int utility(int task) override;
};
#endif
