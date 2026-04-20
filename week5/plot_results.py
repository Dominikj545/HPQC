import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

points = [1000, 10000, 50000, 200000, 500000]
serial = [0.013, 0.111, 0.550, 2.185, 5.353]
parallel = [0.695, 0.713, 1.205, 3.104, 6.586]

plt.figure()
plt.plot(points, serial, 'o-', label='Serial')
plt.plot(points, parallel, 's-', label='Parallel (np=4)')
plt.xlabel('Number of points')
plt.ylabel('Wall clock time (s)')
plt.title('Serial vs Parallel string wave')
plt.legend()
plt.savefig('week5/benchmark.png', dpi=150, bbox_inches='tight')
plt.close()
