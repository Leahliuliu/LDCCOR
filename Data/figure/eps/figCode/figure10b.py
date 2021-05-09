import matplotlib.pyplot as plt
import numpy as np

y1 = [6.95, 7.81, 8.21, 9.53]
y2 = [8.42, 9.06, 8.82, 10.26]
y3 = [7.80, 8.03, 8.84, 9.42]
y4 = [7.33, 8.93, 9.04, 9.89]
x = [2,3,4,5]

arr = np.array([y1, y2, y3, y4])
mean = np.mean(arr, axis=0)
std = np.std(arr, axis=0)

plt.plot(x, y1, 'bo-', markersize=8)
plt.plot(x, y2, 'gv--', color='#00FF00', markersize=8)
plt.plot(x, y3, 'r^--', markersize=8)
plt.plot(x, y4, 'ks--', color='black', markersize=8)

plt.xticks([2,2.5,3,3.5,4,4.5,5], fontsize='14')
plt.yticks([6,7,8,9,10,11], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('K', fontsize=18)
plt.ylabel('Time Cost(s)', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR'], loc='upper left', fontsize=14)

plt.tight_layout()
plt.savefig('../figure10b.eps', dpi=600, format='eps')
plt.show()