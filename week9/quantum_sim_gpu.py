# quantum computer simulator with GPU support
# follows ENCCS tutorial part 3 by Martin Nilsson
# uses PyTorch instead of numpy for GPU acceleration

import numpy as np
import torch as pt
import time

pt.autograd.set_grad_enabled(False)
if pt.cuda.is_available():
    print("GPU available")
else:
    print("no GPU, running on CPU only")

namestack = []

X_gate = np.array([[0, 1],
                   [1, 0]])

Z_gate = np.array([[1, 0],
                   [0,-1]])

H_gate = np.array([[1, 1],
                   [1,-1]]) * np.sqrt(1/2)


def pushQubit(name, weights):
    global workspace, namestack
    if (workspace.shape[0], workspace.shape[1]) == (1, 1):
        namestack = []
    namestack.append(name)
    weights = weights / np.linalg.norm(weights)
    weights = pt.tensor(weights, device=workspace.device,
                        dtype=workspace[0, 0].dtype)
    workspace = pt.reshape(workspace, (1, -1))
    workspace = pt.kron(workspace, weights)

def tosQubit(name):
    global workspace, namestack
    k = len(namestack) - namestack.index(name)
    if k > 1:
        namestack.append(namestack.pop(-k))
        workspace = pt.reshape(workspace, (-1, 2, 2**(k-1)))
        workspace = pt.swapaxes(workspace, -2, -1)

# supports controlled gates
def applyGate(gate, *names):
    global workspace
    if list(names) != namestack[-len(names):]:
        for name in names:
            tosQubit(name)
    workspace = pt.reshape(workspace, (-1, 2**len(names)))
    subworkspace = workspace[:, -gate.shape[0]:]
    gate = pt.tensor(gate.T, device=workspace.device,
                     dtype=workspace[0, 0].dtype)
    if workspace.device.type == 'cuda':
        pt.matmul(subworkspace, gate, out=subworkspace)
    else:
        subworkspace[:, :] = pt.matmul(subworkspace, gate)

def probQubit(name):
    global workspace
    tosQubit(name)
    workspace = pt.reshape(workspace, (-1, 2))
    prob = pt.linalg.norm(workspace, axis=0)**2
    prob = pt.Tensor.cpu(prob).numpy()
    return prob / prob.sum()

def measureQubit(name):
    global workspace, namestack
    prob = probQubit(name)
    measurement = np.random.choice(2, p=prob)
    workspace = workspace[:, [measurement]] / np.sqrt(prob[measurement])
    namestack.pop()
    return str(measurement)


# phase oracles for grover's search

def zero_phaseOracle(qubits):
    for qubit in qubits:
        applyGate(X_gate, qubit)
    applyGate(Z_gate, *namestack)
    for qubit in qubits:
        applyGate(X_gate, qubit)

# finds 111101 for 6 qubits, 1111111111111101 for 16 qubits etc.
def sample_phaseOracle(qubits):
    applyGate(X_gate, qubits[1])
    applyGate(Z_gate, *namestack)
    applyGate(X_gate, qubits[1])

def groverSearch(n, printProb=False):
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

    # quick test with 6 qubits on GPU
    print("\n6 qubit search on GPU")
    workspace = pt.tensor([[1.]], device=pt.device('cuda'),
                          dtype=pt.float32)
    groverSearch(6, printProb=True)
    print("\n")

    # benchmark GPU vs CPU with 16 qubits
    print("16 qubit search on GPU")
    workspace = pt.tensor([[1.]], device=pt.device('cuda'),
                          dtype=pt.float32)
    t = time.process_time()
    groverSearch(16, printProb=False)
    gpu_time = time.process_time() - t
    print(f"\nGPU time: {gpu_time:.2f} s\n")

    print("16 qubit search on CPU")
    workspace = pt.tensor([[1.]], device=pt.device('cpu'),
                          dtype=pt.float32)
    t = time.process_time()
    groverSearch(16, printProb=False)
    cpu_time = time.process_time() - t
    print(f"\nCPU time: {cpu_time:.2f} s\n")

    print(f"speedup: {cpu_time/gpu_time:.1f}x")
