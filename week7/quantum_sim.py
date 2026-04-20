# quantum computer simulator
# following the ENCCS tutorial by Martin Nilsson
# implements the 4 basic stack operations and common gates

import numpy as np

workspace = np.array([[1.]])
namestack = []

# common gates from the tutorial
X_gate = np.array([[0, 1],
                   [1, 0]])

Y_gate = np.array([[ 0,-1j],
                   [1j,  0]])

Z_gate = np.array([[1, 0],
                   [0,-1]])

H_gate = np.array([[1, 1],
                   [1,-1]]) * np.sqrt(1/2)

S_gate = np.array([[1, 0],
                   [0,1j]])

T_gate = np.array([[1,                0],
                   [0,np.exp(np.pi/-4j)]])

Tinv_gate = np.array([[1, 0],
                      [0,np.exp(np.pi/4j)]])

CNOT_gate = np.array([[1, 0, 0, 0],
                      [0, 1, 0, 0],
                      [0, 0, 0, 1],
                      [0, 0, 1, 0]])

CZ_gate = np.array([[1, 0, 0, 0],
                    [0, 1, 0, 0],
                    [0, 0, 1, 0],
                    [0, 0, 0,-1]])

SWAP_gate = np.array([[1, 0, 0, 0],
                      [0, 0, 1, 0],
                      [0, 1, 0, 0],
                      [0, 0, 0, 1]])

TOFF_gate = np.array([[1, 0, 0, 0, 0, 0, 0, 0],
                      [0, 1, 0, 0, 0, 0, 0, 0],
                      [0, 0, 1, 0, 0, 0, 0, 0],
                      [0, 0, 0, 1, 0, 0, 0, 0],
                      [0, 0, 0, 0, 1, 0, 0, 0],
                      [0, 0, 0, 0, 0, 1, 0, 0],
                      [0, 0, 0, 0, 0, 0, 0, 1],
                      [0, 0, 0, 0, 0, 0, 1, 0]])


# push a qubit onto the workspace
# normalises automatically so you can just pass [1,1] for equal probability
def pushQubit(name, weights):
    global workspace, namestack
    if workspace.shape == (1, 1):
        namestack = []
    namestack.append(name)
    weights = weights / np.linalg.norm(weights)
    weights = np.array(weights, dtype=workspace[0, 0].dtype)
    workspace = np.reshape(workspace, (1, -1))
    workspace = np.kron(workspace, weights)

# move qubit to top of stack so we can apply a gate to it
def tosQubit(name):
    global workspace, namestack
    k = len(namestack) - namestack.index(name)
    if k > 1:
        namestack.append(namestack.pop(-k))
        workspace = np.reshape(workspace, (-1, 2, 2**(k-1)))
        workspace = np.swapaxes(workspace, -2, -1)

# apply a gate, can pass qubit names to move them to top first
def applyGate(gate, *names):
    global workspace
    if list(names) != namestack[-len(names):]:
        for name in names:
            tosQubit(name)
    workspace = np.reshape(workspace, (-1, 2**(len(names))))
    subworkspace = workspace[:, -gate.shape[0]:]
    np.matmul(subworkspace, gate.T, out=subworkspace)

# check probability without collapsing, only works in simulator
def probQubit(name):
    global workspace
    tosQubit(name)
    workspace = np.reshape(workspace, (-1, 2))
    prob = np.linalg.norm(workspace, axis=0)**2
    return prob / prob.sum()

# measure and pop a qubit from the stack
def measureQubit(name):
    global workspace, namestack
    prob = probQubit(name)
    measurement = np.random.choice(2, p=prob)
    workspace = workspace[:, [measurement]] / np.sqrt(prob[measurement])
    namestack.pop()
    return str(measurement)


if __name__ == "__main__":

    # push and measure a qubit 30 times
    # 0.6^2 = 0.36 chance of 0, 0.8^2 = 0.64 chance of 1
    print("push and measure 30 times")
    results = ""
    for n in range(30):
        workspace = np.array([[1.]])
        pushQubit("Q1", [0.6, 0.8])
        results += measureQubit("Q1")
    print(results)
    print()

    # NOT gate should flip a zero to a one
    print("NOT gate")
    workspace = np.array([[1.]])
    pushQubit("Q1", [1, 0])
    print("before:", np.reshape(workspace, (1, -1)))
    applyGate(X_gate, "Q1")
    print("after:", np.reshape(workspace, (1, -1)))
    print()

    # Hadamard puts a zero qubit into equal superposition
    print("Hadamard gate")
    workspace = np.array([[1.]])
    pushQubit("Q1", [1, 0])
    print("before:", np.reshape(workspace, (1, -1)))
    applyGate(H_gate, "Q1")
    print("after:", np.reshape(workspace, (1, -1)))
    print()

    # SWAP gate swaps two qubits
    print("SWAP gate")
    workspace = np.array([[1.]])
    pushQubit("Q1", [1, 0])
    pushQubit("Q2", [0.6, 0.8])
    print("before:", np.reshape(workspace, (1, -1)))
    applyGate(SWAP_gate, "Q1", "Q2")
    print("after:", np.reshape(workspace, (1, -1)))
    print()

    # Toffoli gate computes AND
    # two random inputs and a zero output
    print("AND truth table with Toffoli gate")
    workspace = np.array([[1.]])
    for i in range(16):
        pushQubit("Q1", [1, 1])
        pushQubit("Q2", [1, 1])
        pushQubit("Q3", [1, 0])
        applyGate(TOFF_gate, "Q1", "Q2", "Q3")
        q3 = measureQubit("Q3")
        q2 = measureQubit("Q2")
        q1 = measureQubit("Q1")
        print(q1 + q2 + q3, end=" ")
    print("\n")

    # quantum coin flip, should be roughly 50/50
    print("quantum coin flip (100 flips)")
    zeros = 0
    ones = 0
    workspace = np.array([[1.]])
    for i in range(100):
        pushQubit("coin", [1, 0])
        applyGate(H_gate, "coin")
        result = measureQubit("coin")
        if result == "0":
            zeros += 1
        else:
            ones += 1
    print(f"zeros: {zeros}, ones: {ones}")
