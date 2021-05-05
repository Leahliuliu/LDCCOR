import matplotlib.pyplot as plt
import numpy as np

y1 = [9.82, 10.93, 11.96, 13.25]
y2 = [9.53, 11.07, 12.13, 14.25]
y3 = [8.82, 9.56, 10.94, 12.23]
y4 = [8.76, 9.96, 10.64, 12.03]
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
plt.yticks([9,9,10,11,12,13,14,15], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('K', fontsize=18)
plt.ylabel('Energy Cost(W)', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR', 'AVERAGE'], loc='upper left', fontsize=14)

plt.tight_layout()
plt.savefig('./eps/figure12c.eps', dpi=600, format='eps')
plt.show()