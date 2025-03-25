#include "task.hpp"

Task::Task() = default;

Task::~Task() = default;

Task::Task(int id, Point coords, int reward) :
    id(id), coords(coords), reward(reward) {};
