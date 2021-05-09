import matplotlib.pyplot as plt
import numpy as np

y1 = [102.36, 112.62, 128.15, 155.32]
y2 = [185.33, 226.63, 278.36, 298.15]
y3 = [148.23, 212.58, 272.36, 303.86]
y4 = [163.88, 254.61, 292.45, 306.89]
x = [2,3,4,5]

arr = np.array([y1, y2, y3, y4])
mean = np.mean(arr, axis=0)
std = np.std(arr, axis=0)

plt.plot(x, y1, 'bo-', markersize=8)
plt.plot(x, y2, 'gv--', color='#00FF00', markersize=8)
plt.plot(x, y3, 'r^--', markersize=8)
plt.plot(x, y4, 'ks--', color='black', markersize=8)

plt.xticks([2,2.5,3,3.5,4,4.5,5], fontsize='14')
plt.yticks([100,150,200,250,300,350,400], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('K', fontsize=18)
plt.ylabel('Number of Transmissions', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR'], loc='upper left', fontsize=14)

plt.tight_layout()
plt.savefig('../figure10a.eps', dpi=600, format='eps')
plt.show()