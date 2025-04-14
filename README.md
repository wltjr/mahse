# Multi-Agent Hedonic Coalition Formation for Task Allocation Algorithms Comparison
This project aims to implement various multi-agent hedonic coalition formation
for task allocation algorithms, as a course long project in COT6405: Design and
Analysis of Algorithms.

The following algorithms are implemented:
- GRAPE: Anonymous Hedonic Game for Task Allocation in a Large-Scale Multiple
  Agent System [link](https://ieeexplore.ieee.org/abstract/document/8439076)
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
```
