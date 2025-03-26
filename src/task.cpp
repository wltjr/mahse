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

