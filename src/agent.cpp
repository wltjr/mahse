#include <algorithm>
#include <random>
#include <limits>

#include "agent.hpp"

Agent::Agent() = default;

Agent::Agent(int id, int agents, Point coords, std::vector<Task> &tasks, rewards reward) :
    id(id), agents(agents), coords(coords), tasks(tasks), reward(reward) {};

Agent::~Agent() = default;

void Agent::decision()
{
    const int BUFFER_SIZE = 1024;
    const int BUFFER_SIZE_REC = BUFFER_SIZE * agents;
    char buffer_snd[BUFFER_SIZE] = {'0'};
    char buffer_rec[BUFFER_SIZE_REC] = {'0'};
    int position;
    int task_max;
    int tasks_size;
    int utility_cur;
    int utility_max;

    // random functions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> urd(0.0,1.0);

    // Initialization
    satisfied = false;
    iterations = 0;
    seed = 0;
    task = 0;
    utility_cur = std::numeric_limits<int>::min();
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
            utility_max = 0;

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
                auto itr = std::find(partitions[task].begin(), partitions[task].end(), id);
                if(itr != partitions[task].end())
                    partitions[task].erase(itr);
                task = task_max;
                utility_cur = utility_max;

                // Join S_j∗ and update Π
                partitions[task].push_back(id);
                iterations++;
                seed = urd(gen);
            }
            satisfied = true;
        }

        // Broadcast M^i = {r^i , s^i , Π^i } and receive M^k from its neighbors ∀a_k ∈ N_i
        position = 0;
        pack_msg(buffer_snd, BUFFER_SIZE, position, MPI_COMM_WORLD);

        // Broadcast the local information to neighbor agents & receive message
        MPI_Allgather(buffer_snd, position, MPI_CHAR,
                      buffer_rec, position * agents, MPI_CHAR, MPI_COMM_WORLD);

        // Select the valid partition from all the received messages
        // Construct M^i_rcv = {M^i , ∀M^k }
        position = 0;
        std::vector<std::tuple<int, float, std::vector<std::vector<int>>>> msgs =
            unpack_msgs(buffer_rec, BUFFER_SIZE_REC, position, MPI_COMM_WORLD);

        // {r^i , s^i , Π^i }, satisfied = decision_mutex(M^i_rcv)
        // inlined decision_mutex(), no reason for a separate function/method
        satisfied = true;
        for (auto &msg : msgs)
        {
            int r_k;
            float s_k;
            std::vector<std::vector<int>> p_k;

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
    }
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

void Agent::pack_msg(char *buffer, int size, int &position, MPI_Comm comm)
{
    int tasks_size;

    tasks_size = tasks.size();
    MPI_Pack(&iterations, 1, MPI_INT, buffer, size, &position, comm);
    MPI_Pack(&seed, 1, MPI_FLOAT, buffer, size, &position, comm);
    // pack partition coalitions only, we know the partition size, same as tasks
    for(int i = 0; i < tasks_size; i++)
    {
        int coalition_size;

        coalition_size = partitions[i].size();
        // pack size of coalition, then each agent id in the coalition
        MPI_Pack(&coalition_size, 1, MPI_INT, buffer, size, &position, comm);
        for(int  j = 0; j < coalition_size; j++)
            MPI_Pack(&partitions[i][j], 1, MPI_INT, buffer, size, &position, comm);
    }

    MPI_Pack(&tasks_size, 1, MPI_INT, buffer, size, &position, comm);
}

std::vector<std::tuple<int, float, std::vector<std::vector<int>>>>
    Agent::unpack_msgs(char *buffer, int size, int &position, MPI_Comm comm)
{
    int tasks_size;
    std::vector<std::tuple<int, float, std::vector<std::vector<int>>>> msgs;

    tasks_size = tasks.size();
    for(int a = 0; a < agents; a++)
    {
        int r_k;
        float s_k;
        std::vector<std::vector<int>> p_k;

        p_k.resize(tasks_size);
        MPI_Unpack(buffer, size, &position, &r_k, 1, MPI_INT, comm);
        MPI_Unpack(buffer, size, &position, &s_k, 1, MPI_FLOAT, comm);
        // unpack partition coalitions only, we know the partition size, same as tasks
        for(int i = 0; i < tasks_size; i++)
        {
            int coalition_size;

            // unpack size of coalition, then each agent id in the coalition
            MPI_Unpack(buffer, size, &position, &coalition_size, 1, MPI_INT, comm);
            p_k[i].resize(coalition_size);
            for(int  j = 0; j < coalition_size; j++)
                MPI_Unpack(buffer, size, &position, &p_k[i][j], 1, MPI_INT, comm);
        }
        msgs.push_back(std::make_tuple(r_k, s_k, p_k));
    }
    return msgs;
}
