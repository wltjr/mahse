#include <algorithm>
#include <random>
#include <limits>

#include "agent.hpp"

Agent::Agent() = default;

Agent::Agent(int id, Point coords, std::vector<Task> &tasks, rewards reward) :
    id(id), coords(coords), tasks(tasks), reward(reward) {};

Agent::~Agent() = default;

void Agent::decision()
{
    int task_max;
    int tasks_size;
    int utility_cur;
    int utility_max;

    // Initialization
    satisfied = false;
    iterations = 0;
    seed = 0;
    utility_cur = std::numeric_limits<int>::min();
    utility_max = utility_cur;
    tasks_size = tasks.size();
    // empty partition, vector of agent vectors
    // create empty coalitions S_j for all task t_j
    partitions.resize(tasks_size); // Π = {S_0 = A, S_j = ∅ ∀t_j ∈ T }

    // Decision-making process begins
    while(true)
    {
        // Make a new decision if necessary
        if(!satisfied)
        {
            // get the max utility for a given task and participants
            for(int j = 0; j < tasks_size; j++) //(t_j∗, |S_j∗|) = max ∀S j ∈Πi (t_j , |S_j ∪ {a_i}|)
            {
                int value;

                value = utility(j);
                if(value > utility_max)
                {
                    utility_max = value;
                    task_max = j;
                }
            }

            // ≻ strongly preferred preference of max over current
            if(utility_max > utility_cur) // t_j∗, |S_j∗|) ≻_i (t_{Πi(i)} , |S_{Πi (i)}|)
            {
                // leave the current coalition (may need new structure O(N) operation)
                partitions[task].erase(
                    std::find(partitions[task].begin(), partitions[task].end(), this));
                task = task_max;
                utility_cur = utility_max;

                // Join S_j∗ and update Π
                partitions[task].push_back(this);
                iterations++;

                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<> urd(0.0,1.0);
                seed = urd(gen);
            }
            satisfied = true;
        }

        // Broadcast the local information to neighbor agents
        // Broadcast M^i = {r^i , s^i , Π^i } and receive M^k from its neighbors ∀a_k ∈ N_i

        // Select the valid partition from all the received messages
        // Construct M^i_rcv = {M^i , ∀M^k }
        std::vector<std::tuple<int, float, std::vector<std::vector<Agent *>>, bool>> msgs;

        // {r^i , s^i , Π^i }, satisfied = decision_mutex(M^i_rcv)
        const auto msg = decision_mutex(msgs);
        iterations = std::get<0>(msg);
        seed = std::get<1>(msg);
        partitions = std::get<2>(msg);
        satisfied = std::get<3>(msg);
    }
}


std::tuple<int, float, std::vector<std::vector<Agent *>>, bool> Agent::decision_mutex(
    std::vector<std::tuple<int, float, std::vector<std::vector<Agent *>>, bool>> msgs
)
{
    satisfied = true;
    for (auto &msg : msgs)
    {
        int r_k;
        float s_k;
        std::vector<std::vector<Agent *>> p_k;

        r_k = std::get<0>(msg);
        s_k = std::get<1>(msg);
        p_k = std::get<2>(msg);
        if ((r_k > iterations ) ||
            (r_k == iterations && s_k > seed))
        {
            iterations = r_k;
            seed = s_k;
            partitions = p_k;
            satisfied = false;
        }
    }
    return std::make_tuple(iterations, seed, partitions, satisfied);
}

int Agent::distance(int task)
{
    int x;
    int y;

    Point task_coords = tasks[task].get_coords();
    x = coords.x - task_coords.x;
    y = coords.y - task_coords.y;

    return std::sqrt(x * x + y * y);
}

int Agent::reward_peaked(int task)
{
    int participants;
    int desired;

    participants = partitions[task].size();
    desired = tasks[task].get_modifier();

    // add 1 to any task agent is not assigned to, simulate joining
    if(task != this->task)
        participants++;

    return (tasks[task].get_reward() * participants) / desired *
            std::pow(std::exp(1.0), -participants / desired + 1);
            // std::pow(std::numbers::e, -participants / desired + 1)
}

int Agent::reward_submodular(int task)
{
    int participants;
    int epsilon;

    participants = partitions[task].size();
    epsilon = tasks[task].get_modifier();

    // add 1 to any task agent is not assigned to, simulate joining
    if(task != this->task)
        participants++;

    return tasks[task].get_reward() *
           (std::log(participants + epsilon - 1) / std::log(epsilon))  // log(n) / log(base)
           / participants;
}

int Agent::utility(int task)
{
    int value;

    switch(reward)
    {
        case peaked:
            value = reward_peaked(task);
            break;
        default:
            value = reward_submodular(task);
            break;
    }

    return value - distance(task);
}
