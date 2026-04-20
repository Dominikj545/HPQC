# HPQC - High Performance and Quantum Computing Portfolio

PHY1090 Individual Practical Portfolio

Dominik Jakobczak

Physics with Data Analytics (PAN4), DCU

## Overview

This repository contains my practical work for the PHY1090 High Performance and Quantum Computing module. Weeks 2-5 cover high performance computing with C and MPI. Weeks 7-9 cover quantum computing with Python.

I connected to the Cheetah and Frank HPC servers during week 1 to verify access and run initial tests. For the rest of the exercises I used my local desktop since it has significantly more computing power. I set up SSH access to my desktop through a Raspberry Pi with wake-on-LAN so I could work on it remotely.

## Weeks

**Week 2 - Performance and Parallelism**
Benchmarking C vs Python across computation and I/O operations. Wrote file read/write timing programs in both languages.

**Week 3 - Introduction to MPI**
First MPI programs. Analysed proof.c, built a parallel vector addition, benchmarked serial vs parallel.

**Week 4 - MPI Communications**
Tested MPI send types, wrote a pingpong program to measure latency and bandwidth, implemented collective communications (Broadcast, Scatter, Gather, Reduce).

**Week 5 - Communicators and Topologies**
Simulated vibrations on a string. Removed hard-coded values, parallelised with MPI, improved the physics with a spring-mass model.

**Week 7 - Quantum Computer Simulator**
Built a quantum computer simulator in Python following Part 1 of the ENCCS tutorial. Implements push, gate, move, and measure operations on a qubit stack.

**Week 8 - Grover's Search**
Implemented Grover's search algorithm on the simulator following Part 2 of the ENCCS tutorial. Demonstrated quadratic speedup for unstructured search.

**Week 9 - GPU Quantum Simulator**
Adapted the simulator to run on GPU using PyTorch following Part 3 of the ENCCS tutorial. Benchmarked GPU vs CPU with a 54x speedup on an RTX 3090.

## Systems

**Cheetah (DCU HPC server)** - Dual Intel Xeon E5-2620 v4, 32 threads, 32GB RAM, no compute GPU. Used for initial access verification.

**Frank (DCU HPC server)** - Intel Core i3-3240, 4 threads, 32GB RAM, no compute GPU.

**Local desktop (Pop!_OS)** - AMD Ryzen 9 5950X, 128GB RAM, dual NVIDIA RTX 3090s with NVLink. Used for all benchmarking and development. Accessible remotely via SSH through a Raspberry Pi with wake-on-LAN.
