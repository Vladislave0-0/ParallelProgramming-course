import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("./visualization_output/time_results.txt")
n_procs = data[:6, 0]
times = data[:6, 1]

speedup = times[0] / times
efficiency = speedup / n_procs * 100

plt.figure(figsize=(12, 5))

plt.subplot(1, 2, 1)
plt.plot(n_procs, times, 's--', label="Измеренное время")
plt.xlabel("Число процессов")
plt.ylabel("Время (с)")
plt.title("Время выполнения")
plt.grid(True)

plt.subplot(1, 2, 2)
plt.plot(n_procs, speedup, '.g--', label="Ускорение (S)")
plt.xlabel("Число процессов")
plt.title("Ускорение")
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.savefig("./visualization_output/scaling_results.png")
# plt.show()
plt.close()
