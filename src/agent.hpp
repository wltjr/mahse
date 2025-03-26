#ifndef AGENT_HPP
#define AGENT_HPP

#include <tuple>
#include <vector>

#include "task.hpp"

class Agent
{
    public:
        /**
         * @brief Construct a new Agent object, empty/unused
         */
        Agent();

        /**
         * @brief Construct a new Agent object
         *
         * @param id the task id
         * @param coords the task coordinates (x,y)
         * @param tasks vector of tasks
         */
        Agent(int id, Point coords, std::vector<Task> &tasks);

        /**
         * @brief Euclidean distance between task and agent
         */
        int distance(int task);

        /**
         * @brief Destroy the Agent object, empty/unused
         */
        virtual ~Agent();

    private:
        int id;
        Point coords;
        bool satisfied;
        std::vector<std::vector<Agent>> partitions;  // Π
        std::vector<Task> tasks; // T
        int iterations; // r
        float seed; // s [0,1] uniform-random variable

        /**
         * @brief Decentralized decision-making algorithm
         */
        void decision();

        /**
         * @brief Distributed Mutex Subroutine
         * 
         * @param msgs a vector of tuples, (iterations, seed, partition, satisfied)
         * 
         * @return a tuple (iterations, seed, partition, satisfied)
         */
        std::tuple<int, float, std::vector<std::vector<Agent>>, bool> decision_mutex(
            std::vector<std::tuple<int, float, std::vector<std::vector<Agent>>, bool>> msgs
        );
};
#endif
