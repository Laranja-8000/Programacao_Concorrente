
from re import split
from turtle import circle


file = open("output.txt",'r')

N = 2 # Numero de particulas
pos = [[] for x in range(N)]
# Usar [[]]*N cria erros de referência. NUNCA usar

cont = 0
for l in file.readlines():
    #print("Inserindo em pos["+str(cont%N)+"]: "+str(l[:-1].split(' ')))
    #print(pos[0],end='\n')
    pos[cont%N].append( l[:-1].split(' '))
    cont+=1
    #if(cont<10): print(pos)
for i in pos:
    for j in range(len(i)):
        i[j] = [float(i[j][0]),float(i[j][1])]      

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation

plt.style.use('dark_background')
fig = plt.figure(figsize=(20,20), dpi=80)
fig.patch.set_facecolor('black')
fig.canvas.manager.full_screen_toggle()
ax = plt.axes(xlim=(-10.0, 10.0), ylim=(-10.0, 10.0))
ax.set_aspect(1)

circle = [None]*N
colors = ['green','red','cyan','yellow','pink']

for i in range(N):
    circle[i] = plt.Circle((pos[i][0][0],pos[i][0][1]), 0.08, color=colors[i%N], lw=2.5, zorder = 20)

for i in range(N):
    ax.add_patch(circle[i])

def animate(i):
    #for j in range(N):
    #    circle[j].center = pos[j][i][0], pos[j][i][1]

    #print(len(pos[0]))
    #print(str(pos[0][i][0])+', '+str(pos[0][i][1]))
    circle[0].center = pos[0][i][0], pos[0][i][1]
    circle[1].center = pos[1][i][0], pos[1][i][1]

    #circle[0].center = pos1[i][0], pos1[i][1]
    #circle[1].center = pos2[i][0], pos2[i][1]
    #line[j].set_xdata(par[j].sol_pos.x[:i+1])
    #line[j].set_ydata(par[j].sol_pos.y[:i+1])

anim = FuncAnimation(fig, animate, frames=len(pos[0]), interval=16, blit=False)

plt.show()
