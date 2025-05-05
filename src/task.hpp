#ifndef TASK_HPP
#define TASK_HPP

#include <mpi.h>

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
        Point get_coords() const;

        /**
         * @brief Id getter
         *
         * @return the tasks id
         */
        int get_id() const;

        /**
         * @brief Modifier getter
         *
         * @return the tasks reward modifier value
         */
        int get_modifier() const;

        /**
         * @brief Reward getter
         *
         * @return the tasks reward value
         */
        int get_reward() const;

        /**
         * @brief Destroy the Task object, empty/unused
         */
        virtual ~Task();

        /**
         * @brief Pack a class instance into contiguous memory for MPI
         *
         * @param buffer pointer to char buffer used to store instance
         * @param size buffer size, in bytes (non-negative integer)
         * @param position current position in buffer, in bytes (integer)
         * @param comm 	communicator for packed message (handle)
         */
        void pack(char *buffer, int size, int &position, MPI_Comm comm);

        /**
         * @brief Unpack a class instance stored contiguous memory from MPI
         *
         * @param buffer pointer to char buffer used to store instance
         * @param size buffer size, in bytes (non-negative integer)
         * @param position current position in buffer, in bytes (integer)
         * @param comm 	communicator for packed message (handle)
         */
        void unpack(const char *buffer, int size, int &position, MPI_Comm comm);

    private:
        int id;
        Point coords;
        int reward; // max or min for peaked or submodular
        int modifier; // either n_j^d for peaked or ε_j for submodular
};

#endif
