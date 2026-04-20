# Week 7 - Quantum Computer Simulator

This exercise follows Part 1 of the ENCCS "Program your own quantum computer" tutorial by Martin Nilsson. I implemented a quantum computer simulator in Python using numpy. The simulator models a quantum computer as a stack machine where qubits are pushed on, gates are applied, and qubits are measured and popped off.

## Running the code

```
python3 quantum_sim.py
```

No arguments needed. It runs through a set of tests showing each operation working.

## How it works

The simulator has four core operations that match the tutorial's stack machine model.

`pushQubit` adds a qubit to the workspace by taking the Kronecker product of the current workspace with the new qubit's weight vector. This doubles the size of the workspace each time. Weights are normalised automatically so you can just pass [1,1] for equal probability instead of calculating the square root.

`tosQubit` moves a named qubit to the top of the stack using numpy's swapaxes. This is needed because gates always act on the top qubits.

`applyGate` performs a matrix multiplication of a gate on the top qubits in the stack. It can take qubit names as arguments to move them to the top first. It also has an optimisation where it skips the move if the stack is already in the right order.

`measureQubit` calculates the probability of the qubit being 0 or 1, randomly picks one based on those probabilities, then collapses the workspace and pops the qubit off the stack.

There is also `probQubit` which lets you check probabilities without collapsing. This is impossible on a real quantum computer but useful for debugging in the simulator.

## Gates

I defined the common gates from the tutorial. Single qubit gates include X (NOT), Y, Z, Hadamard, S, T and inverse T. Two qubit gates include CNOT, CZ and SWAP. The Toffoli gate operates on three qubits.

The X gate flips a qubit (NOT operation). The Hadamard gate puts a zero qubit into equal superposition. The Toffoli gate computes AND on three qubits.

## Tests

The tests at the bottom of the file demonstrate each operation.

The push and measure test creates a qubit with 0.36 probability of 0 and 0.64 probability of 1, then measures it 30 times. The results should show roughly 36% zeros and 64% ones.

The NOT gate test shows a zero qubit being flipped to one.

The Hadamard test shows a zero qubit going into equal superposition with weights of roughly 0.707 each.

The SWAP test shows two qubits having their positions exchanged.

The AND truth table uses a Toffoli gate with two random inputs and a zero output. Only the cases where both inputs are 1 produce a 1 in the output.

The coin flip test applies a Hadamard to a zero qubit 100 times and measures it each time. The results should be roughly 50/50 between zeros and ones.
