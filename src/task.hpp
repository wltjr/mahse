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
         * @param reward the task reward
         */
        Task(int id, Point coords, int reward);

        /**
         * @brief Destroy the Task object, empty/unused
         */
        virtual ~Task();

    private:
        int id;
        Point coords;
        int reward;
};

#endif
