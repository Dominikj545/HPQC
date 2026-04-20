# grover's search implementation
# follows ENCCS tutorial part 2 by Martin Nilsson

import numpy as np

workspace = np.array([[1.]])
namestack = []

X_gate = np.array([[0, 1],
                   [1, 0]])

Z_gate = np.array([[1, 0],
                   [0,-1]])

H_gate = np.array([[1, 1],
                   [1,-1]]) * np.sqrt(1/2)

CNOT_gate = np.array([[1, 0, 0, 0],
                      [0, 1, 0, 0],
                      [0, 0, 0, 1],
                      [0, 0, 1, 0]])

TOFF_gate = np.array([[1, 0, 0, 0, 0, 0, 0, 0],
                      [0, 1, 0, 0, 0, 0, 0, 0],
                      [0, 0, 1, 0, 0, 0, 0, 0],
                      [0, 0, 0, 1, 0, 0, 0, 0],
                      [0, 0, 0, 0, 1, 0, 0, 0],
                      [0, 0, 0, 0, 0, 1, 0, 0],
                      [0, 0, 0, 0, 0, 0, 0, 1],
                      [0, 0, 0, 0, 0, 0, 1, 0]])


def pushQubit(name, weights):
    global workspace, namestack
    if workspace.shape == (1, 1):
        namestack = []
    namestack.append(name)
    weights = weights / np.linalg.norm(weights)
    weights = np.array(weights, dtype=workspace[0, 0].dtype)
    workspace = np.reshape(workspace, (1, -1))
    workspace = np.kron(workspace, weights)

def tosQubit(name):
    global workspace, namestack
    k = len(namestack) - namestack.index(name)
    if k > 1:
        namestack.append(namestack.pop(-k))
        workspace = np.reshape(workspace, (-1, 2, 2**(k-1)))
        workspace = np.swapaxes(workspace, -2, -1)

# supports controlled gates
def applyGate(gate, *names):
    global workspace
    if list(names) != namestack[-len(names):]:
        for name in names:
            tosQubit(name)
    workspace = np.reshape(workspace, (-1, 2**(len(names))))
    subworkspace = workspace[:, -gate.shape[0]:]
    np.matmul(subworkspace, gate.T, out=subworkspace)

def probQubit(name):
    global workspace
    tosQubit(name)
    workspace = np.reshape(workspace, (-1, 2))
    prob = np.linalg.norm(workspace, axis=0)**2
    return prob / prob.sum()

def measureQubit(name):
    global workspace, namestack
    prob = probQubit(name)
    measurement = np.random.choice(2, p=prob)
    workspace = workspace[:, [measurement]] / np.sqrt(prob[measurement])
    namestack.pop()
    return str(measurement)


# flips sign when all qubits are zero
def zero_phaseOracle(qubits):
    for qubit in qubits:
        applyGate(X_gate, qubit)
    applyGate(Z_gate, *namestack)
    for qubit in qubits:
        applyGate(X_gate, qubit)

# the target function, finds 111101 for 6 qubits
def sample_phaseOracle(qubits):
    applyGate(X_gate, qubits[1])
    applyGate(Z_gate, *namestack)
    applyGate(X_gate, qubits[1])

# main grover loop
def groverSearch(n, printProb=True):
    optimalTurns = int(np.pi/4 * np.sqrt(2**n) - 1/2)
    qubits = list(range(n))
    for qubit in qubits:
        pushQubit(qubit, [1, 1])
    for k in range(optimalTurns):
        sample_phaseOracle(qubits)
        for qubit in qubits:
            applyGate(H_gate, qubit)
        zero_phaseOracle(qubits)
        for qubit in qubits:
            applyGate(H_gate, qubit)
        if printProb:
            print(probQubit(qubits[0]))
    for qubit in reversed(qubits):
        print(measureQubit(qubit), end="")


if __name__ == "__main__":

    # 6 qubit search, should find 111101
    print("grover's search with 6 qubits")
    workspace = np.array([[1.]])
    groverSearch(6)
    print()
    print()

    # run it a few more times to check consistency
    print("5 more runs:")
    for i in range(5):
        workspace = np.array([[1.]])
        groverSearch(6, printProb=False)
        print()

    print()

    # 8 qubits, should find 11111101
    print("grover's search with 8 qubits")
    workspace = np.array([[1.]])
    groverSearch(8, printProb=True)
    print()
