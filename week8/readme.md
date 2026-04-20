# Week 8 - Grover's Search

This exercise follows Part 2 of the ENCCS tutorial. I implemented Grover's search algorithm on top of the quantum simulator from week 7.

## Running the code

```
python3 grovers_search.py
```

No arguments needed. It runs the search with 6 qubits and 8 qubits and prints the convergence probabilities at each iteration.

## How it works

Grover's search finds a specific input to a function in O(sqrt(N)) time instead of O(N) for a classical search. It does this by repeatedly amplifying the probability of the correct answer.

The algorithm needs two phase oracles. A phase oracle flips the sign of the workspace weights for a particular state instead of returning a value.

`zero_phaseOracle` flips the sign when all qubits are zero. It negates all qubits with X gates, applies a controlled Z gate across the whole stack, then undoes the negation.

`sample_phaseOracle` is the target function. It identifies the state where all qubits except qubit 1 are one. For 6 qubits that means 111101. It only needs to negate qubit 1 and apply the controlled Z because every other qubit is already supposed to be one.

The main loop in `groverSearch` starts by pushing n qubits in equal superposition. Then it alternates between applying the sample oracle and the zero oracle with Hadamard gates in between. Each iteration pushes the probabilities closer to the target state. The number of iterations is calculated from the formula k = floor(pi/4 * sqrt(2^n) - 1/2).

## Results

With 6 qubits the probabilities for qubit 0 converge over 5 iterations. The probability of measuring 1 goes from about 0.56 to 0.98 by the last iteration. The output is 111101 which matches the target.

Running it 5 more times gives 111101 four times and a wrong answer once. This is expected because the result is probabilistic. You would check the answer and redo the search if it was wrong.

With 8 qubits the search takes 12 iterations and finds 11111101. The convergence is slower because there are more states to search through but it still gets there.

The algorithm gives a quadratic speedup over classical search, not exponential. But it can be shown that this is the best any quantum algorithm can do for an unstructured search.
