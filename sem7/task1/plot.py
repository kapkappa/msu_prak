import numpy as np
import matplotlib.pyplot  as plt
maxtime = 0
n = 0
x = []
y = []
with open('log') as f:
    maxtime = int(f.readline())
    n = int(f.readline())
    timestep = float(f.readline())
    for i in range(n):
        t,v = map(float, f.readline().split())
        x.append(t)
        y.append(v)

plt.figure()
plt.plot(x, y)
plt.xlabel('Время')
plt.ylabel('Вероятнотсть')
plt.suptitle(f'dt: {timestep}')
plt.show()

