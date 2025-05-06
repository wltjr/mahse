#include <algorithm>
#include <array>
#include <format>
#include <iostream>
#include <random>
#include <limits>
#include <sstream>

#include "grape.hpp"

Grape::Grape() = default;

Grape::Grape(int id, int agents, Point coords, std::vector<Task> &tasks, rewards reward)
{
    this->id = id;
    this->agents = agents;
    this->coords = coords;
    this->tasks = tasks;
    this->reward = reward;
}

Grape::~Grape() = default;

void Grape::decision()
{
    const int BUFFER_SIZE = 1024;
    const int BUFFER_SIZE_REC = BUFFER_SIZE * agents;
    std::array<char, BUFFER_SIZE> buffer_snd = {'0'};
    std::vector<char> buffer_rec(BUFFER_SIZE_REC, '0');
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
    task_max = 0;
    utility_cur = std::numeric_limits<int>::min();
    tasks_size = tasks.size();
    // empty partition, vector of agent vectors
    // create empty coalitions S_j for all task t_j
    partitions.resize(tasks_size); // Π = {S_0 = A, S_j = ∅ ∀t_j ∈ T }
    for(int i = 1; i <= agents; i++)
        partitions[0].push_back(i);

    // Decision-making process begins
    for(int i = 0;i <= iterations; i++)
    {
        if(!satisfied) // Make a new decision if necessary
        {
            utility_max = std::numeric_limits<int>::min();

            // get the max utility for a given task and participants
            for(int j = 1; j < tasks_size; j++) //(t_j∗, |S_j∗|) = max ∀S j ∈Πi (t_j , |S_j ∪ {a_i}|)
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
                if(auto itr = std::find(partitions[task].begin(), partitions[task].end(), id);
                   itr != partitions[task].end())
                    partitions[task].erase(itr);
                task = task_max;
                utility_cur = utility_max;
                partitions[task].push_back(id); // Join S_j∗ and update Π
                iterations++;
                seed = urd(gen);
            }
            satisfied = true;
        }

        // Broadcast M^i = {r^i , s^i , Π^i } and receive M^k from its neighbors ∀a_k ∈ N_i
        position = 0;
        pack_msg(&buffer_snd[0], BUFFER_SIZE, position, MPI_COMM_WORLD);

        // Broadcast the local information to neighbor agents & receive message
        MPI_Allgather(&buffer_snd[0], position, MPI_CHAR,
                      &buffer_rec[0], position, MPI_CHAR, MPI_COMM_WORLD);

        // Select the valid partition from all the received messages
        // Construct M^i_rcv = {M^i , ∀M^k }
        std::vector<std::tuple<int, double, std::vector<std::vector<int>>>> msgs =
            unpack_msgs(&buffer_rec[0], BUFFER_SIZE_REC, position, MPI_COMM_WORLD);

        // {r^i , s^i , Π^i }, satisfied = decision_mutex(M^i_rcv)
        // inlined decision_mutex(), no reason for a separate function/method
        satisfied = true;
        for (auto &msg : msgs)
        {
            int r_k;
            double s_k;
            std::vector<std::vector<int>> p_k;

            tie(r_k, s_k, p_k) = msg;
            if ((r_k > iterations ) || (r_k == iterations && s_k > seed))
            {
                iterations = r_k;
                seed = s_k;
                partitions = p_k;
                satisfied = false;

                // set agent's task and utility from partition coalition update
                for(auto &t: tasks)
                {
                    int t_id;

                    t_id = t.get_id();
                    auto itr = std::find(partitions[t_id].begin(), partitions[t_id].end(), id);
                    if(itr != partitions[t_id].end())
                    {
                        task = t_id;
                        if(task == 0)
                            utility_cur = std::numeric_limits<int>::min();
                        else
                            utility_cur = utility(t_id);
                        break;
                    }
                }
            }
        }
    }

    // display final agent information
    std::cout << std::format("Agent: {}, task: {}, utility: {}, iterations: {}",
                             id, task, utility_cur, iterations) << std::endl;

    MPI_Barrier(MPI_COMM_WORLD);
    if(id == 1)
    {
        std::stringstream result;

        result << "Final partition:" << std::endl;
        for(int i = 0; i < tasks_size; i++)
        {
            int size;

            size = partitions[i].size();
            result << std::format("Coalition: {}, Size: {}, Agent Ids: ", i, size);
            for(int  j = 0; j < size; j++)
                result << partitions[i][j] << ',';
            result << std::endl;
        }

        std::cout << result.str() << std::endl;
    }
}

int Grape::utility(int task)
{
    int value;
    int participants;
    int modifier;

    participants = partitions[task].size();
    modifier = tasks[task].get_modifier();

    // add 1 to any task agent is not assigned to, simulate joining
    if(task != this->task)
        participants++;

    switch(reward)
    {
        // Peaked Reward
        case peaked:
            value = (tasks[task].get_reward() * participants) / modifier *
                     std::pow(std::exp(1.0), -participants / modifier + 1);
                    // std::pow(std::numbers::e, -participants / desired + 1)
            break;
        // Submodular Reward
        default:
            value = tasks[task].get_reward() *
                    (std::log(participants + modifier - 1) / std::log(modifier))  // log(n) / log(base)
                    / participants;
            break;
    }

    return value - distance(task);
}

void Grape::pack_msg(char *buffer, int size, int &position, MPI_Comm comm)
{
    int tasks_size;

    tasks_size = tasks.size();
    MPI_Pack(&iterations, 1, MPI_INT, buffer, size, &position, comm);
    MPI_Pack(&seed, 1, MPI_DOUBLE, buffer, size, &position, comm);
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

std::vector<std::tuple<int, double, std::vector<std::vector<int>>>>
    Grape::unpack_msgs(char *buffer, int size, int &position, MPI_Comm comm)
{
    int tasks_size;
    std::vector<std::tuple<int, double, std::vector<std::vector<int>>>> msgs;

    tasks_size = tasks.size();
    for(int a = 0; a < agents; a++)
    {
        int r_k;
        int pos;
        double s_k;
        std::vector<std::vector<int>> p_k;

        pos = a * position;
        p_k.resize(tasks_size);
        MPI_Unpack(buffer, size, &pos, &r_k, 1, MPI_INT, comm);
        MPI_Unpack(buffer, size, &pos, &s_k, 1, MPI_DOUBLE, comm);
        // unpack partition coalitions only, we know the partition size, same as tasks
        for(int i = 0; i < tasks_size; i++)
        {
            int coalition_size;

            // unpack size of coalition, then each agent id in the coalition
            MPI_Unpack(buffer, size, &pos, &coalition_size, 1, MPI_INT, comm);
            p_k[i].resize(coalition_size);
            for(int  j = 0; j < coalition_size; j++)
                MPI_Unpack(buffer, size, &pos, &p_k[i][j], 1, MPI_INT, comm);
        }
        msgs.emplace_back(std::make_tuple(r_k, s_k, p_k));
    }
    return msgs;
}
