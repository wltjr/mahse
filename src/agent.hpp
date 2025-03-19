#ifndef AGENT_HPP
#define AGENT_HPP

#include <tuple>
#include <vector>

class Agent
{
    public:
        /**
         * @brief Construct a new Agent object, empty/unused
         */
        Agent();

        /**
         * @brief Destroy the Agent object, empty/unused
         */
        virtual ~Agent();

    private:
        bool satisfied;
        int partition;  // Π
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
        std::tuple<int, float, int, bool> decision_mutex(
            std::vector<std::tuple<int, float, int, bool>> msgs
        );
};
#endif
