import matplotlib.pyplot as plt
import numpy as np

y1 = [5.59, 5.83, 6.09, 6.52]
y2 = [5.71, 6.02, 6.91, 7.37]
y3 = [5.43, 6.16, 6.82, 7.23]
y4 = [6.18, 6.31, 6.67, 7.07]
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
plt.yticks([5,5.5,6,6.5,7,7.5], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('K', fontsize=18)
plt.ylabel('Time Cost(s)', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR', 'AVERAGE'], loc='upper left', fontsize=14)

plt.tight_layout()
plt.savefig('./eps/figure12b.eps', dpi=600, format='eps')
plt.show()