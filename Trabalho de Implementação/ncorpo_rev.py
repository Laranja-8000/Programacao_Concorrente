import numpy as np
import matplotlib.pyplot as plt

n = 4
gravity = 0.0001
step_size = 20/1000
half_step_size = 0.5 * step_size
softening = 0.1

#position = 4 * (np.random.rand(n, 2) - 0.5)
position = []
offset = -0.0
position.append([0+offset,0+offset])
position.append([1.0+offset,0+offset])
position.append([0+offset,1.0+offset])
position.append([-1.0+offset,0+offset])
position = np.array(position)

#velocity = 1 * (np.random.rand(n, 2) - 0.5)
velocity = []
velocity.append([0.0,0.0])
velocity.append([0.0,3.0])
velocity.append([3.0,0.0])
velocity.append([.0,3.0])
velocity = np.array(velocity)

acceleration = np.zeros((n, 2))
mass = 500 * np.ones((n, 1))

com_p = np.sum(np.multiply(mass, position), axis=0) / np.sum(mass, axis=0)
com_v = np.sum(np.multiply(mass, velocity), axis=0) / np.sum(mass, axis=0)
for p in position: p -= com_p
for v in velocity: v -= com_v

# particula central (posição zero):
position[0] = np.array([0, 0])
velocity[0] = np.array([0, 0])
mass[0] = 40000

plt.style.use('dark_background')
fig = plt.figure(figsize=(20,20), dpi=80)
fig.patch.set_facecolor('black')
fig.canvas.manager.full_screen_toggle()
ax = plt.axes(xlim=(-2.5, 2.5), ylim=(-2.5, 2.5))
ax.set_aspect(1)
scatter = ax.scatter(position[:,0], position[:,1], np.sqrt(mass), 
                     color="#56c474", edgecolors='black', lw=1)


def calculate_acceleration_matrix(P, M, gravity, softening):
    x = P[:,0:1]
    y = P[:,1:2]
    dx = x.T - x
    dy = y.T - y
    inv_r3 = (dx**2 + dy**2 + softening**2) ** (-1.5)
    ax = gravity * (dx * inv_r3) @ M
    ay = gravity * (dy * inv_r3) @ M
    return np.hstack((ax, ay))

from matplotlib.animation import FuncAnimation

def animate(i):
    global acceleration, velocity, position, softening 
    global step_size, half_step_size

    velocity += acceleration * half_step_size
    position += velocity * step_size
    
    acceleration = calculate_acceleration_matrix(position, mass, gravity, softening)

    velocity += acceleration * half_step_size
    scatter.set_offsets(position)


anim = FuncAnimation(fig, animate, frames=400, interval=20, blit=False)
plt.show()
