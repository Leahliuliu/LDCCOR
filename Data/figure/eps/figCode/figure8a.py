import matplotlib.pyplot as plt
import numpy as np

y1 = [108.73, 195.84, 304.28, 300.08]
y2 = [258.17, 347.61, 483.23, 564.73]
y3 = [236.72, 483.33, 598.26, 585.69]
y4 = [272.35, 389.53, 603.46, 608.97]
x = [60, 90, 120, 150]

arr = np.array([y1, y2, y3, y4])
mean = np.mean(arr, axis=0)
std = np.std(arr, axis=0)

plt.plot(x, y1, 'bo-', markersize=8)
plt.plot(x, y2, 'gv--', color='#00FF00', markersize=8)
plt.plot(x, y3, 'r^--', markersize=8)
plt.plot(x, y4, 'ks--', color='black', markersize=8)

plt.xticks([60, 80, 100, 120, 140], fontsize='14')
plt.yticks([100, 200, 300, 400, 500, 600, 100], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('Number of Nodes', fontsize=18)
plt.ylabel('Number of Transmissions', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR'], loc='upper left', fontsize=14)

plt.tight_layout()
plt.savefig('../figure8a.eps', dpi=600, format='eps')
plt.show()