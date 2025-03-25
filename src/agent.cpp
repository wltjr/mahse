#include <random>

#include "agent.hpp"

Agent::Agent() = default;

Agent::Agent(int id, Point coords, std::vector<Task> &tasks) :
    id(id), coords(coords), tasks(tasks) {
        // create empty coalitions Sj for each task Tj
        partitions.resize(tasks.size());
};

Agent::~Agent() = default;

void Agent::decision()
{
    // Initialization
    satisfied = false;
    iterations = 0;
    seed = 0;
    // empty set of partitions, vector of agent vectors
    partitions.clear(); // Π = {S_0 = A, S_j = ∅ ∀t_j ∈ T }

    // Decision-making process begins
    while(true)
    {
        // Make a new decision if necessary
        if(!satisfied)
        {
            //(t_j∗, |S_j∗|) = max ∀S j ∈Πi (t_j , |S_j ∪ {a_i}|)

            // temp conditional replace with commented conditional code
            if(iterations) // t_j∗, |S_j∗|) >_i (t_{Πi(i)} , |S_{Πi (i)}|)
            {
                // Join S_j∗ and update Π
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
        std::vector<std::tuple<int, float, std::vector<std::vector<Agent>>, bool>> msgs;

        // {r^i , s^i , Π^i }, satisfied = decision_mutex(M^i_rcv)
        const auto msg = decision_mutex(msgs);
        iterations = std::get<0>(msg);
        seed = std::get<1>(msg);
        partitions = std::get<2>(msg);
        satisfied = std::get<3>(msg);
    }
}


std::tuple<int, float, std::vector<std::vector<Agent>>, bool> Agent::decision_mutex(
    std::vector<std::tuple<int, float, std::vector<std::vector<Agent>>, bool>> msgs
)
{
    satisfied = true;
    for (auto &msg : msgs)
    {
        int r_k;
        float s_k;
        std::vector<std::vector<Agent>> p_k;

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
