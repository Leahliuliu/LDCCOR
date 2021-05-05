import matplotlib.pyplot as plt
import numpy as np

y1 = [48.17, 52.31, 61.25, 72.36]
y2 = [49.03, 57.21, 75.13, 82.26]
y3 = [52.34, 61.59, 77.84, 92.07]
y4 = [50.86, 62.41, 79.33, 89.85]
x = [2,3,4,5]

arr = np.array([y1, y2, y3, y4])
mean = np.mean(arr, axis=0)
std = np.std(arr, axis=0)

plt.errorbar(x, mean, yerr=std, fmt="*", color="black", ecolor='black', elinewidth=2, capsize=16, markersize=12)
plt.plot(x, y1, 'bo-', markersize=8)
plt.plot(x, y2, 'gv--', color='#00FF00', markersize=8)
plt.plot(x, y3, 'r^--', markersize=8)
plt.plot(x, y4, 'ks--', color='pink', markersize=8)
plt.xticks([2,2.5,3,3.5,4,4.5,5], fontsize='14')
plt.yticks([40,50,60,70,80,90,100], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('K', fontsize=18)
plt.ylabel('Number of Transmissions', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR', 'AVERAGE'], loc='upper left', fontsize=14)

plt.tight_layout()
plt.savefig('./eps/figure12a.eps', dpi=600, format='eps')
plt.show()