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
         * @param coords the task coordinates (x,y)
         * @param tasks vector of tasks
         * @param reward the reward heuristic to use
         */
        Agent(int id, Point coords, std::vector<Task> &tasks, rewards reward);

        /**
         * @brief Euclidean distance between task and agent
         *
         * @param task the task index number, e.g. the j in (t_j, S_j)
         */
        int distance(int task);

        /**
         * @brief Peaked-reward function
         *
         * @param task the task index number, e.g. the j in (t_j, S_j)
         */
        int reward_peaked(int task);

        /**
         * @brief Submodular reward function
         *
         * @param task the task index number, e.g. the j in (t_j, S_j)
         */
        int reward_submodular(int task);

        /**
         * @brief Utility function
         *
         * @param task the task index number, e.g. the j in (t_j, S_j)
         */
        int utility(int task);

        /**
         * @brief Destroy the Agent object, empty/unused
         */
        virtual ~Agent();

    private:
        int id;
        Point coords;
        bool satisfied;
        std::vector<std::vector<int>> partitions;  // Π
        std::vector<Task> tasks; // T
        int iterations; // r
        float seed; // s [0,1] uniform-random variable
        int reward;
        int task; // the task index number, e.g. the j in (t_j, S_j)

        /**
         * @brief Decentralized decision-making algorithm
         */
        void decision();
};
#endif
