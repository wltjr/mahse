#include "task.hpp"

Task::Task() = default;

Task::~Task() = default;

Task::Task(int id, Point coords, int reward, int modifier) :
    id(id), coords(coords), reward(reward), modifier(modifier) {};

Point Task::get_coords()
{
    return(coords);
}

int Task::get_id()
{
    return(id);
}

int Task::get_modifier()
{
    return(modifier);
}

int Task::get_reward()
{
    return(reward);
}

void Task::pack(char *buffer, int size, int &position, MPI_Comm comm)
{
    MPI_Pack(&id, 1, MPI_INT, buffer, size, &position, comm);
    MPI_Pack(&coords.x, 1, MPI_INT, buffer, size, &position, comm);
    MPI_Pack(&coords.y, 1, MPI_INT, buffer, size, &position, comm);
    MPI_Pack(&reward, 1, MPI_INT, buffer, size, &position, comm);
    MPI_Pack(&modifier, 1, MPI_INT, buffer, size, &position, comm);
}

void Task::unpack(char *buffer, int size, int &position, MPI_Comm comm)
{
    MPI_Unpack(buffer, size, &position, &id, 1, MPI_INT, comm);
    MPI_Unpack(buffer, size, &position, &coords.x, 1, MPI_INT, comm);
    MPI_Unpack(buffer, size, &position, &coords.y, 1, MPI_INT, comm);
    MPI_Unpack(buffer, size, &position, &reward, 1, MPI_INT, comm);
    MPI_Unpack(buffer, size, &position, &modifier, 1, MPI_INT, comm);
}
