#ifndef AGENT_HPP
#define AGENT_HPP

#include <tuple>
#include <vector>

#include "point.hpp"

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
         */
        Agent(int id, Point coords);

        /**
         * @brief Destroy the Agent object, empty/unused
         */
        virtual ~Agent();

    private:
        int id;
        Point coords;
        bool satisfied;
        std::vector<std::vector<Agent>> partitions;  // Π
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
