import matplotlib.pyplot as plt
import numpy as np

y1 = [15.85, 13.78, 13.13, 12.05, 10.16, 11.84, 12.68, 11.97, 10.25]
y2 = [24.23, 23.87, 23.56, 22.21, 22.42, 17.48, 15.03, 15.22, 15.76]
y3 = [15.94, 15.89, 15.14, 15.65, 15.73, 13.81, 11.61, 12.46, 13.07]
y4 = [16.29, 16.37, 16.41, 17.13, 18.47, 14.34, 10.69, 10.06, 9.82]
x = [0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8]

arr = np.array([y1, y2, y3, y4])
mean = np.mean(arr, axis=0)
std = np.std(arr, axis=0)

plt.errorbar(x, mean, yerr=std, fmt="*", color="black", ecolor='black', elinewidth=2, capsize=16, markersize=12)
plt.plot(x, y1, 'bo-', markersize=8)
plt.plot(x, y2, 'gv--', color='#00FF00', markersize=8)
plt.plot(x, y3, 'r^--', markersize=8)
plt.plot(x, y4, 'ks--', color='pink', markersize=8)
plt.xticks([0.4, 0.5, 0.6, 0.7, 0.8], fontsize='14')
plt.yticks([10, 15, 20, 25], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('Link Quality', fontsize=18)
plt.ylabel('Energy Cost(W)', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR', 'AVERAGE'], loc='upper right', fontsize=14)

plt.tight_layout()
plt.savefig('./eps/figure7c.eps', dpi=600, format='eps')
plt.show()