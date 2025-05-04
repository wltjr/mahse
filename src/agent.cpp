#include <random>

#include "agent.hpp"

Agent::Agent() = default;

Agent::~Agent() = default;

int Agent::distance(int task)
{
    int x;
    int y;

    Point task_coords = tasks[task].get_coords();
    x = coords.x - task_coords.x;
    y = coords.y - task_coords.y;

    return std::sqrt(x * x + y * y);
}
