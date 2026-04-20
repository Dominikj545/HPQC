# Week 9 - GPU Quantum Computer Simulator

This exercise follows Part 3 of the ENCCS tutorial. I adapted the quantum simulator from the previous weeks to run on a GPU using PyTorch instead of numpy. Then I benchmarked GPU vs CPU performance using Grover's search.

## Running the code

Needs PyTorch installed. Run with:
```
python3 quantum_sim_gpu.py
```

It runs a 6 qubit search with convergence output, then benchmarks 16 qubits on both GPU and CPU.

## Changes from numpy to PyTorch

The main change is replacing numpy functions with their PyTorch equivalents. `np.reshape` becomes `pt.reshape`, `np.kron` becomes `pt.kron`, `np.matmul` becomes `pt.matmul` and so on. The workspace is created as a PyTorch tensor with a device parameter that controls whether it lives in GPU memory or CPU memory.

The `probQubit` function needs to pull the probability data back to the CPU with `pt.Tensor.cpu()` because `np.random.choice` only works with numpy arrays. This is a small amount of data (just two numbers) so the transfer is fast.

There is a workaround in `applyGate` for a PyTorch bug with `matmul` on CPU. On GPU it can write directly to the output tensor but on CPU it needs to go through a temporary copy.

## Results

The 6 qubit search works the same as before and finds 111101 with the same convergence pattern.

For the 16 qubit benchmark the GPU finished in 1.09 seconds while the CPU took 59.53 seconds. That is a 54.4x speedup.

| Device | 16 qubit time | Speedup |
|---|---|---|
| GPU (RTX 3090) | 1.09 s | 54.4x |
| CPU | 59.53 s | 1x |

The GPU is faster because the workspace operations (Kronecker products, matrix multiplications, reshaping) are all parallel operations that GPUs handle well. The workspace at 16 qubits has 2^16 = 65536 elements and the gate operations touch all of them at once, which is exactly the kind of workload GPUs are built for. There is also very little data transfer needed between CPU and GPU since the workspace stays in GPU memory for the entire search.
