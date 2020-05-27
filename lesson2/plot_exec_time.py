import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

output = np.loadtxt('./result.tsv', delimiter='\t')
n3 = np.zeros(len(output))
n3[0] = output[0, 1]
for i in np.arange(1, len(output)):
    n3[i] = (i + 1) ** 3 * n3[0]

sns.set()
plt.plot(output[:, 0], output[:, 1], label='Actual Execution Time')
plt.plot(output[:, 0], n3, label='Theoretical Execution Time (' + r'$N^3$' + ')')
plt.xlabel('Size of the Matrix')
plt.ylabel('Execution Time [msec]')
plt.legend()
plt.show()
