import matplotlib.pyplot as plt
import numpy as np

y1 = [10.13, 13.27, 15.59, 19.48]
y2 = [16.32, 18.27, 21.96, 25.31]
y3 = [17.31, 17.83, 20.14, 22.09]
y4 = [17.62, 19.25, 20.43, 23.34]
x = [800,900,1000,1100]

arr = np.array([y1, y2, y3, y4])
mean = np.mean(arr, axis=0)
std = np.std(arr, axis=0)

plt.plot(x, y1, 'bo-', markersize=8)
plt.plot(x, y2, 'gv--', color='#00FF00', markersize=8)
plt.plot(x, y3, 'r^--', markersize=8)
plt.plot(x, y4, 'ks--', color='black', markersize=8)

plt.xticks([800,850,900,950,1000,1050,1100], fontsize='14')
plt.yticks([10,15,20,25,30], fontsize='14')
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.xlabel('Side Length(m)', fontsize=18)
plt.ylabel('Energy Cost(W)', fontsize=18)
plt.legend(['LDC-COR', 'LDC-OR', 'LDC-ACOR', 'LDC-AOR'], loc='upper left', fontsize=14)

plt.tight_layout()
plt.savefig('../figure9c.eps', dpi=600, format='eps')
plt.show()