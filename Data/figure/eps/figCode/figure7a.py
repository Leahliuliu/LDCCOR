import matplotlib.pyplot as plt
import numpy as np

y1 = [151.32, 148.54, 138.72, 122.45, 119.12, 122.23, 137.53, 128.02, 124.71]
y2 = [268.75, 255.32, 248.98, 224.31, 204.77, 151.36, 140.23, 130.74, 126.25]
y3 = [298.78, 269.35, 249.71, 240.53, 234.16, 175.31, 152.84, 150.25, 143.42]
y4 = [317.23, 300.09, 289.28, 282.37, 253.43, 220.19, 183.24, 161.47, 149.88]
x = [0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8]

arr = np.array([y1, y2, y3, y4])
mean = np.mean(arr, axis=0)
std = np.std(arr, axis=0)

plt.plot(x, y1, 'bo-', markersize=8)
plt.plot(x, y2, 'gv--', color='#00FF00', markersize=8)
plt.plot(x, y3, 'r^--', markersize=8)
plt.plot(x, y4, 'ys--', color='black', markersize=8)

plt.xticks([0.4, 0.5, 0.6, 0.7, 0.8], fontsize='14')
plt.yticks([100, 150, 200, 250, 300, 350], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('Link Quality', fontsize=18)
plt.ylabel('Number of Transmissions', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR'], loc='upper right', fontsize=14)

plt.tight_layout()
plt.savefig('../figure7a.eps', dpi=600, format='eps')
plt.show()

