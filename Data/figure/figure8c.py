import matplotlib.pyplot as plt
import numpy as np

y1 = [11.67, 39.43, 122.56, 161.39]
y2 = [23.76, 63.49, 123.53, 191.27]
y3 = [16.34, 60.26, 98.31, 138.77]
y4 = [19.56, 51.61, 115.89, 163.74]
x = [60, 90, 120, 150]

arr = np.array([y1, y2, y3, y4])
mean = np.mean(arr, axis=0)
std = np.std(arr, axis=0)

plt.errorbar(x, mean, yerr=std, fmt="*", color="black", ecolor='black', elinewidth=2, capsize=16, markersize=12)
plt.plot(x, y1, 'bo-', markersize=8)
plt.plot(x, y2, 'gv--', color='#00FF00', markersize=8)
plt.plot(x, y3, 'r^--', markersize=8)
plt.plot(x, y4, 'ks--', color='pink', markersize=8)
plt.xticks([60, 80, 100, 120, 140], fontsize='14')
plt.yticks([50, 100, 150, 200], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('Number of Nodes', fontsize=18)
plt.ylabel('Energy Cost(W)', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR', 'AVERAGE'], loc='upper left', fontsize=14)

plt.tight_layout()
plt.savefig('./eps/figure8c.eps', dpi=600, format='eps')
plt.show()