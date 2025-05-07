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
         * @brief Decision-making algorithm
         */
        virtual void decision() = 0;

        /**
         * @brief Destroy the Agent object, empty/unused
         */
        virtual ~Agent();

    protected:
        int id;
        int agents;
        Point coords;
        std::vector<Task> tasks;
        int reward;
        int task; // the task index number, e.g. the j in (t_j, S_j)

        /**
         * @brief Euclidean distance between task and agent
         *
         * @param task the task index number, e.g. the j in (t_j, S_j)
         */
        int distance(int task);

        /**
         * @brief Utility function
         *
         * @param task the task index number, e.g. the j in (t_j, S_j)
         */
        virtual int utility(int task) = 0;
};
#endif
