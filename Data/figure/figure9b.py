import matplotlib.pyplot as plt
import numpy as np

y1 = [6.83, 7.42, 8.06, 8.38]
y2 = [8.18, 8.33, 9.08, 9.75]
y3 = [8.19, 7.92, 8.21, 8.78]
y4 = [7.82, 8.54, 9.29, 9.67]
x = [800,900,1000,1100]

arr = np.array([y1, y2, y3, y4])
mean = np.mean(arr, axis=0)
std = np.std(arr, axis=0)

plt.errorbar(x, mean, yerr=std, fmt="*", color="black", ecolor='black', elinewidth=2, capsize=16, markersize=12)
plt.plot(x, y1, 'bo-', markersize=8)
plt.plot(x, y2, 'gv--', color='#00FF00', markersize=8)
plt.plot(x, y3, 'r^--', markersize=8)
plt.plot(x, y4, 'ks--', color='pink', markersize=8)
plt.xticks([800,850,900,950,1000,1050,1100], fontsize='14')
plt.yticks([7,8,9,10], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('Side Length(m)', fontsize=18)
plt.ylabel('Time Cost(s)', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR', 'AVERAGE'], loc='upper left', fontsize=14)

plt.tight_layout()
plt.savefig('./eps/figure9b.eps', dpi=600, format='eps')
plt.show()