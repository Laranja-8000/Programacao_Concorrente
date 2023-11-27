
from re import split
from turtle import circle


file = open("output.txt",'r')

pos1 = []
pos2 = []

alternate = 0
for line in file.readlines():
    if alternate == 0:
        pos1.append(line[:-1].split(' '))
        alternate = 1
    else:
        pos2.append(line[:-1].split(' '))
        alternate = 0
for i in range(len(pos1)):
    pos1[i] = [float(pos1[i][0]),float(pos1[i][1])]
    pos2[i] = [float(pos2[i][0]),float(pos2[i][1])]


#print(pos1)

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation

plt.style.use('dark_background')
fig = plt.figure(figsize=(20,20), dpi=80)
fig.patch.set_facecolor('black')
fig.canvas.manager.full_screen_toggle()
ax = plt.axes(xlim=(-10.0, 10.0), ylim=(-10.0, 10.0))
ax.set_aspect(1)

circle = [None]*2
circle[0] = plt.Circle((pos1[0][0], pos1[0][1]), 0.08, color='green', ec="green", lw=2.5, zorder=20)
ax.add_patch(circle[0])
circle[1] = plt.Circle((pos2[0][0], pos2[0][1]), 0.08, color='red', ec="red", lw=2.5, zorder=20)
ax.add_patch(circle[1])


def animate(i):
    circle[0].center = pos1[i][0], pos1[i][1]
    circle[1].center = pos2[i][0], pos2[i][1]
        #line[j].set_xdata(par[j].sol_pos.x[:i+1])
        #line[j].set_ydata(par[j].sol_pos.y[:i+1])

anim = FuncAnimation(fig, animate, frames=len(pos1), interval=100, blit=False)

plt.show()