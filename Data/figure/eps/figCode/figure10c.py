import matplotlib.pyplot as plt
import numpy as np

y1 = [9.98, 12.81, 21.32, 29.3]
y2 = [19.12, 20.08, 25.03, 35.09]
y3 = [13.04, 15.02, 17.56, 27.8]
y4 = [10.33, 16.8, 20.96, 26.93]
x = [2,3,4,5]

arr = np.array([y1, y2, y3, y4])
mean = np.mean(arr, axis=0)
std = np.std(arr, axis=0)

plt.plot(x, y1, 'bo-', markersize=8)
plt.plot(x, y2, 'gv--', color='#00FF00', markersize=8)
plt.plot(x, y3, 'r^--', markersize=8)
plt.plot(x, y4, 'ks--', color='black', markersize=8)

plt.xticks([2,2.5,3,3.5,4,4.5,5], fontsize='14')
plt.yticks([5,10,15,20,25,30,35,40], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('K', fontsize=18)
plt.ylabel('Energy Cost(W)', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR'], loc='upper left', fontsize=14)

plt.tight_layout()
plt.savefig('../figure10c.eps', dpi=600, format='eps')
plt.show()