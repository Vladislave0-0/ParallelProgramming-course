import pandas as pd
import matplotlib.pyplot as plt

# Читаем CSV
data = pd.read_csv("./results/variant_1a/data_speedup.csv")

# График ускорения
plt.figure(figsize=(7,5))
plt.plot(data["p"], data["Speedup"], marker="o", label="Ускорение S(p)")
plt.plot(data["p"], data["p"], "--", color="gray", label="Идеальное ускорение S=p")
plt.title("Зависимость ускорения от числа потоков")
plt.xlabel("Число потоков p")
plt.ylabel("Ускорение S(p)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("speedup.png", dpi=200)
