#ifndef TASK_HPP
#define TASK_HPP

#include "point.hpp"

class Task
{
    public:
        /**
         * @brief Construct a new Task object, empty/unused
         */
        Task();

        /**
         * @brief Construct a new Task object
         * 
         * @param id the task id
         * @param coords the task coordinates (x,y)
         * @param reward the task reward, max or min for peaked or submodular
         * @param modifier the reward modifier, n_j^d for peaked or ε_j for submodular
         */
        Task(int id, Point coords, int reward, int modifier);

        /**
         * @brief Coords getter
         *
         * @return the tasks coordinates (x,y)
         */
        Point get_coords();

        /**
         * @brief Id getter
         *
         * @return the tasks id
         */
        int get_id();

        /**
         * @brief Modifier getter
         *
         * @return the tasks reward modifier value
         */
        int get_modifier();

        /**
         * @brief Reward getter
         *
         * @return the tasks reward value
         */
        int get_reward();

        /**
         * @brief Destroy the Task object, empty/unused
         */
        virtual ~Task();

    private:
        int id;
        Point coords;
        int reward; // max or min for peaked or submodular
        int modifier; // either n_j^d for peaked or ε_j for submodular
};

#endif
