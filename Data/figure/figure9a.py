import matplotlib.pyplot as plt
import numpy as np

y1 = [110.77, 141.32, 163.28, 242.83]
y2 = [188.45, 245.36, 252.13, 331.46]
y3 = [141.38, 203.72, 243.47, 318.56]
y4 = [173.21, 246.32, 270.49, 350.23]
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
plt.yticks([100,150,200,250,300,350], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('Side Length(m)', fontsize=18)
plt.ylabel('Number of Transmissions', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR', 'AVERAGE'], loc='upper left', fontsize=14)

plt.tight_layout()
plt.savefig('./eps/figure9a.eps', dpi=600, format='eps')
plt.show()
