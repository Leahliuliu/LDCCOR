import matplotlib.pyplot as plt

x = [4, 5, 6, 7, 8, 9, 10]

k2_greedy = [2.25,2.25,2.25,2.11,2,2,2]
k2_optimal = [2.25,2.25,2.25,2.11,2,2,2]
k2_bad = [6,6,6,6,6,6,6]

k3_greedy = [4.10,3.68,3.68,3.54,3.43,3.11,3.11]
k3_optimal = [4.10,3.68,3.68,3.36,3.22,3.00,3.00]
k3_bad = [7.68,7.68,8.5,8.5,8.5,8.5,8.5]

k4_greedy = [9.29,5.78,5.63,5.08,4.79,4.54,4.36]
k4_optimal = [9.29,5.71,5.52,5.04,4.76,4.36,4.22]
k4_bad = [9.29,9.35,10.18,11,11,11,11]

k5_greedy = [9.29,10.95,8.27,7.72,7.40,6.79,6.65]
k5_optimal = [9.29,10.95,7.62,7.06,6.65,6.22,5.90]
k5_bad = [9.29,10.95,11.85,12.68,13.5,13.5,13.5]

plt.figure(figsize=(8, 6))

#把plt.subplot(411)改成221
#41意思是4行1列, 22是2行2列

plt.subplot(221)
plt.title('K=2', fontsize='14')
plt.xlabel("Number of Neighbours", fontsize='14')
plt.ylabel("ETX", fontsize='14')
plt.plot(x, k2_greedy, 'r-^', markersize=7)
plt.plot(x, k2_optimal, 'b--p',markersize=7)
plt.plot(x, k2_bad, 'g-*',color='#00FF00',markersize=7)
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.legend(['Greedy', 'Optimal', 'Worst'], loc='center right', fontsize=14)

plt.subplot(222)
plt.title('K=3', fontsize='14')
plt.xlabel("Number of Neighbours", fontsize='14')
plt.ylabel("ETX", fontsize='14')
plt.plot(x, k3_greedy, 'r-^', markersize=10)
plt.plot(x, k3_optimal, 'b--p', markersize=10)
plt.plot(x, k3_bad, 'g-*',color='#00FF00', markersize=10)
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.legend(['Greedy', 'Optimal', 'Worst'], loc='center right', fontsize=14)

plt.subplot(223)
plt.title('K=4', fontsize='14')
plt.xlabel("Number of Neighbours", fontsize='14')
plt.ylabel("ETX", fontsize='14')
plt.plot(x, k4_greedy, 'r-^', markersize=10)
plt.plot(x, k4_optimal, 'b--p', markersize=10)
plt.plot(x, k4_bad, 'g-*',color='#00FF00', markersize=10)
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.legend(['Greedy', 'Optimal', 'Worst'], loc='center right', fontsize=14)

plt.subplot(224)
plt.title('K=5', fontsize='14')
plt.xlabel("Number of Neighbours", fontsize='14')
plt.ylabel("ETX", fontsize='14')
plt.plot(x, k5_greedy, 'r-^', markersize='10')
plt.plot(x, k5_optimal, 'b--p', markersize=10)
plt.plot(x, k5_bad, 'g-*',color='#00FF00', markersize=10)
plt.grid(linewidth=0.4, color='#DFDFDF')
plt.legend(['Greedy', 'Optimal', 'Worst'], loc='center right', fontsize=14)

plt.tight_layout()
plt.savefig('./eps/figure14.eps', dpi=600, format='eps')
plt.show()


