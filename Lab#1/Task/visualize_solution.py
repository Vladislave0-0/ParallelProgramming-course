import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.animation import FuncAnimation

T = 1.0
X = 2.0
M = 1000
K = 1000
h = X / M
tau = T / K

# Начальное условие u(0,x) = \phi(x)
x = np.linspace(0, X, M+1)
phi = np.sin(np.pi * x)

plt.figure(figsize=(10, 6))
plt.plot(x, phi, linewidth=2)
plt.title("Начальное условие: u(0,x) = sin(πx)")
plt.xlabel("x")
plt.ylabel("u(0,x)")
plt.grid(True)
plt.savefig("./solution_output/initial_condition.png")
# plt.show()
plt.close()

# Граничного условие u(t,0) = \psi(t)
t = np.linspace(0, T, K+1)
psi = np.sin(2 * np.pi * t / T)

plt.figure(figsize=(10, 6))
plt.plot(t, psi, linewidth=2, color='orange')
plt.title("Граничное условие: u(t,0) = sin(2πt/T)")
plt.xlabel("t")
plt.ylabel("u(t,0)")
plt.grid(True)
plt.savefig("./solution_output/boundary_condition.png")
# plt.show()
plt.close()

# Все временные слои загружаем из full_solution.dat 
u_all = np.loadtxt("./solution_output/full_solution.dat")

X_grid, T_grid = np.meshgrid(x, t)
# Отрисовка решения в виде 3D визуализации
fig = plt.figure(figsize=(12, 8))
ax = fig.add_subplot(111, projection='3d')
surf = ax.plot_surface(X_grid, T_grid, u_all, cmap=cm.viridis)
ax.set_xlabel('Пространство (x)')
ax.set_ylabel('Время (t)')
ax.set_zlabel('u(t,x)')
ax.set_title("Решение уравнения переноса")
plt.colorbar(surf)
plt.savefig("./solution_output/3d_solution.png")
# plt.show()
plt.close()

# Отрисовка решения в виде .gif
fig_anim, ax_anim = plt.subplots(figsize=(10, 6))

def update(frame):
    ax_anim.clear()
    ax_anim.plot(x, u_all[frame], 'b-', linewidth=2)
    ax_anim.set_xlim(0, X)
    ax_anim.set_ylim(np.min(u_all), np.max(u_all))
    ax_anim.set_title(f"Решение при t = {frame*tau:.2f}")
    ax_anim.set_xlabel("x")
    ax_anim.set_ylabel("u(t,x)")
    ax_anim.grid(True)

frames = np.linspace(0, K, 50, dtype=int)
anim = FuncAnimation(fig_anim, update, frames=frames, interval=100)
anim.save("./solution_output/solution_animation.gif", writer='pillow', fps=10)
plt.close(fig_anim)

print("\033[32mВизуализация завершена. Созданы файлы в директории ./solution_output/:\033[0m")
print("   - Начального условия (initial_condition.png)")
print("   - Граничного условия (boundary_condition.png)")
print("   - 3D графика решения (3d_solution.png)")
print("   - Анимации решения   (solution_animation.gif)")
