#!/bin/bash

# Peaked-reward based on paper 60 and 15
r0=28		# reward
m0=7 		# agents

# Submodular reward from paper
r1=10		# reward
m1=2		# design param

# simulation and experiments
dim=1000	# grid dimensions
agents=28   # max number of agents to test, starts at 5
tasks=5     # number of tasks to test, starts at 2
count=5 	# loop iterations

# variables for commands
mpi_cmd="mpirun --use-hwthread-cpus --host dev:16,ws:12"
my_cmd="./mahse -d {dim}"

# create logs dir and empty log files
mkdir -p logs
rm logs/*log

echo "Running GRAPE Experiments"
echo
for a in $(seq 5 ${agents}); do
    for t in $(seq 2 ${tasks}); do
        echo "Testing ${a} Agents with ${t} Tasks ${count} times"
        avg_iter0=0
        avg_iter1=0
        avg_time0=0
        avg_time1=0
        for i in $(seq $count); do
            file0="logs/agents${a}_tasks${t}_peaked_exp_${i}.log"
            file1="logs/agents${a}_tasks${t}_submodular_exp_${i}.log"

            ${mpi_cmd} -np ${a} ${my_cmd} -u 0 -r ${r0} -m ${m0} > ${file0}
            ${mpi_cmd} -np ${a} ${my_cmd} -u 1 -r ${r1} -m ${m1} > ${file1}

            etime0=$(grep "Elapsed time" $file0 | cut -d ' ' -f 3)
            etime1=$(grep "Elapsed time" $file1 | cut -d ' ' -f 3)

            iter0=$(grep -m1 "iterations" $file0 | cut -d ' ' -f 8)
            iter1=$(grep -m1 "iterations" $file1 | cut -d ' ' -f 8)

            avg_iter0=$((${avg_iter0} + ${iter0}))
            avg_iter1=$((${avg_iter1} + ${iter1}))

            avg_time0=$(bc -l <<< "${avg_time0} + ${etime0/s/}")
            avg_time1=$(bc -l <<< "${avg_time1} + ${etime1/s/}")
        done
        avg_iter0=$((${avg_iter0} / ${count}))
        avg_iter1=$((${avg_iter1} / ${count}))

        avg_time0=$(bc -l <<< "${avg_time0} / ${count}")
        avg_time1=$(bc -l <<< "${avg_time1} / ${count}")

        printf "Peaked Elapsed time: %.10f, Iterations: %d \n" ${avg_time0} ${avg_iter0}
        printf "Submodular Elapsed time: %.10f, Iterations: %d \n" ${avg_time1} ${avg_iter1}
        echo
    done
done