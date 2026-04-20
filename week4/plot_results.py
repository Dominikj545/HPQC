import matplotlib
import matplotlib.pyplot as plt
import numpy as np

# latency convergence
pings = [10, 100, 1000, 10000, 100000, 1000000]
avg_time = [2.22e-06, 6.80e-07, 3.37e-07, 2.76e-07, 2.39e-07, 2.34e-07]

plt.figure()
plt.plot(pings, avg_time, 'o-')
plt.xscale('log')
plt.xlabel('Number of pings')
plt.ylabel('Average round trip time (s)')
plt.title('Latency convergence')
plt.savefig('week4/latency.png', dpi=150, bbox_inches='tight')
plt.close()

# bandwidth
bytes_sent = [8, 64, 512, 4096, 32768, 262144, 2097152]
bw_time = [2.89e-07, 3.25e-07, 5.14e-07, 3.72e-06, 8.63e-06, 4.27e-05, 2.82e-04]

coeffs = np.polyfit(bytes_sent, bw_time, 1)
m = coeffs[0]
c = coeffs[1]
fit_x = np.linspace(0, max(bytes_sent), 100)
fit_y = m * fit_x + c

bandwidth = 1.0 / m
latency = c

plt.figure()
plt.plot(bytes_sent, bw_time, 'o', label='Measured')
plt.plot(fit_x, fit_y, '--', label=f'Linear fit (BW={bandwidth/1e9:.2f} GB/s, Lat={latency*1e6:.2f} us)')
plt.xlabel('Message size (bytes)')
plt.ylabel('Average round trip time (s)')
plt.title('Bandwidth measurement')
plt.legend()
plt.savefig('week4/bandwidth.png', dpi=150, bbox_inches='tight')
plt.close()

print(f"Latency (y-intercept): {latency*1e6:.2f} microseconds")
print(f"Bandwidth (1/slope): {bandwidth/1e9:.2f} GB/s")
