from re import split
from turtle import circle

file = open("output.txt", 'r')

# Número de partículas (Sol e os oito planetas)
N = 9

pos = [[] for _ in range(N)]

cont = 0
for l in file.readlines():
    pos[cont % N].append(l[:-1].split(' '))
    cont += 1

for i in pos:
    for j in range(len(i)):
        i[j] = [float(i[j][0]), float(i[j][1])]

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Ajusta os limites do gráfico para a escala do sistema solar
fig = plt.figure(figsize=(10, 10), dpi=80)
ax = plt.axes(xlim=(-5.5e12, 5.5e12), ylim=(-5.5e12, 5.5e12))
ax.set_aspect('equal')

# Cria os círculos das partículas
circles = [plt.Circle((pos[i][0][0], pos[i][0][1]), 2e10, color='blue', zorder=20) for i in range(N)]

# Adiciona os círculos ao gráfico
for circle in circles:
    ax.add_patch(circle)

def animate(i):
    # Atualiza as posições do Sol e dos planetas
    for j in range(N):
        circles[j].center = pos[j][i][0], pos[j][i][1]

# Cria a animação
anim = FuncAnimation(fig, animate, frames=len(pos[0]), interval=16, blit=False)

# Exibe o gráfico
plt.show()