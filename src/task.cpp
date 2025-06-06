#include "task.hpp"

Task::Task() = default;

Task::~Task() = default;

Task::Task(int id, Point coords, int reward, int modifier) :
    id(id), coords(coords), reward(reward), modifier(modifier) {};

Point Task::get_coords() const
{
    return coords;
}

int Task::get_id() const
{
    return id;
}

int Task::get_modifier() const
{
    return modifier;
}

int Task::get_reward() const
{
    return reward;
}

void Task::pack(char *buffer, int size, int &position, MPI_Comm comm) const
{
    MPI_Pack(&id, 1, MPI_INT, buffer, size, &position, comm);
    MPI_Pack(&coords.x, 1, MPI_INT, buffer, size, &position, comm);
    MPI_Pack(&coords.y, 1, MPI_INT, buffer, size, &position, comm);
    MPI_Pack(&reward, 1, MPI_INT, buffer, size, &position, comm);
    MPI_Pack(&modifier, 1, MPI_INT, buffer, size, &position, comm);
}

void Task::unpack(const char *buffer, int size, int &position, MPI_Comm comm)
{
    MPI_Unpack(buffer, size, &position, &id, 1, MPI_INT, comm);
    MPI_Unpack(buffer, size, &position, &coords.x, 1, MPI_INT, comm);
    MPI_Unpack(buffer, size, &position, &coords.y, 1, MPI_INT, comm);
    MPI_Unpack(buffer, size, &position, &reward, 1, MPI_INT, comm);
    MPI_Unpack(buffer, size, &position, &modifier, 1, MPI_INT, comm);
}
