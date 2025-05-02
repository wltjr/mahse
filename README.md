# Multi-Agent Hedonic Simulation Environment
[![License](http://img.shields.io/badge/license-MIT-blue.svg?colorB=9977bb&style=plastic)](https://github.com/wltjr/mahse/blob/master/LICENSE)
![Build Status](https://github.com/wltjr/mahse/actions/workflows/build.yml/badge.svg)
[![Code Quality](https://sonarcloud.io/api/project_badges/measure?project=wltjr_mahse&metric=alert_status)](https://sonarcloud.io/dashboard?id=wltjr_mahse)

This project aims to implement various multi-agent hedonic coalition formation
for task allocation algorithms for research purposes.

The following algorithms are implemented:
- GRAPE: Anonymous Hedonic Game for Task Allocation in a Large-Scale Multiple
  Agent System [link](https://ieeexplore.ieee.org/abstract/document/8439076)

The following algorithms are planning to be implemented:
- GRAPE-S: Near Real-Time Coalition Formation for Multiple Service Collectives
  [link](https://arxiv.org/abs/2310.12480)

## System Requirements
The following software is required for proper operation

  [GCC >= 14.1](https://gcc.gnu.org/releases.html)  
  [CMake >= 3.20](https://cmake.org/download/)  
  [OpenMPI >= 4.1](https://www.open-mpi.org/software/)

# Build
Run the following commands in the root directory of the repository to compile
all executables. The base project uses cmake build system with default of make.
```bash
cmake ./
make
```

# Clean
To remove all executables and build files run the following command in the root
directory of the repository. 
```bash
make clean
```
Note: This will not remove cmake cache files, only files created by make.

# Run
The primary executable is `mahse` multi-agent hedonic simulation environment.
The program is implemented using GNU Argp, and has available `--help` menu for
information on the arguments that each program accepts, which are required and
are optional. This is a MPI program, which means it requires running under
`mpirun`. The number of MPI processes determines the number of agents, tasks
start at 2 by default, minimum run is 3 nodes/agents, and 2 tasks.

```bash
# run with 5 agents, 2 tasks (default)
mpirun -np 5 ./mahse

# run with 20 agents across 2 machines(28 max) reward 20, modifier 5, tasks 5, Peaked-Reward
mpirun --use-hwthread-cpus -np 20 --host host1:16,host2:12 ./mahse -r 20 -m 5 -t 5 -u 0
```

# Experiments
A script has been created `exp/grape_exp.sh` that will run grape against a variety
of agent, tasks, and heuristics and collect data. Values are hard coded but
variables can be changed at the top for easy of modification for experimentation.
Log files will be generated for each run in a `logs/` subdirectory of what ever
directory the script is run in, typically, the root directory of the project,
where the binary is located as it will not be found otherwise.
```bash
# run in project dir
./exp/grape_exp.sh
```
