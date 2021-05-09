import matplotlib.pyplot as plt
import numpy as np

y1 = [7.62, 7.51, 7.44, 7.25, 6.96, 7.10, 7.28, 7.16, 6.85]
y2 = [7.7, 7.66, 7.76, 7.72, 7.46, 7.39, 7.31, 7.23, 7.14]
y3 = [7.95, 7.9, 8.04, 7.87, 7.75, 7.48, 7.38, 7.45, 7.4]
y4 = [7.51, 7.49, 7.41, 7.65, 7.68, 7.51, 7.42, 7.20, 6.95]
x = [0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8]

arr = np.array([y1, y2, y3, y4])
mean = np.mean(arr, axis=0)
std = np.std(arr, axis=0)

plt.plot(x, y1, 'bo-', markersize=8)
plt.plot(x, y2, 'gv--', color='#00FF00', markersize=8)
plt.plot(x, y3, 'r^--', markersize=8)
plt.plot(x, y4, 'ks--', color='black', markersize=8)

plt.xticks([0.4, 0.5, 0.6, 0.7, 0.8], fontsize='14')
plt.yticks([7, 7.5, 8], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('Link Quality', fontsize=18)
plt.ylabel('Time Cost(s)', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR'], loc='upper right', fontsize=14)

plt.tight_layout()
plt.savefig('../figure7b.eps', dpi=600, format='eps')
plt.show()
