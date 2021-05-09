import matplotlib.pyplot as plt
import numpy as np

y1 = [7.68, 9.26, 10.17, 10.39]
y2 = [9.52, 10.12, 10.54, 12.71]
y3 = [8.07, 9.92, 10.47, 10.96]
y4 = [9.05, 10.12, 11.89, 12.02]
x = [60, 90, 120, 150]

arr = np.array([y1, y2, y3, y4])
mean = np.mean(arr, axis=0)
std = np.std(arr, axis=0)

plt.plot(x, y1, 'bo-', markersize=8)
plt.plot(x, y2, 'gv--', color='#00FF00', markersize=8)
plt.plot(x, y3, 'r^--', markersize=8)
plt.plot(x, y4, 'ks--', color='black', markersize=8)

plt.xticks([60, 80, 100, 120, 140], fontsize='14')
plt.yticks([7, 8, 9, 10, 11, 12, 13], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('Number of Nodes', fontsize=18)
plt.ylabel('Time Cost(s)', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR'], loc='upper left', fontsize=14)

plt.tight_layout()
plt.savefig('../figure8b.eps', dpi=600, format='eps')
plt.show()