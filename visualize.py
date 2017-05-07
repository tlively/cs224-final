import matplotlib.pyplot as plt 
import numpy as np

times = []
timed_out_nodes = [0 for i in range(12,26)]
nodes = {}

with open('series.out') as f:
	lines = f.readlines()
	for line in lines:
		l = line.split(',')
		l = l[1:] # we don't care about the test file
		node = int(l[0])
		m = int(l[1])
		schedule_length = int(l[2])
		scheduling_time = float(l[3])

		if m != 16:
			continue
		
		if schedule_length < 0:
			timed_out_nodes[node - 12] += 1
		
		times.append(scheduling_time)
		if node in nodes.keys():
			nodes[node].append(scheduling_time)
		else:
			nodes[node] = []

print times
print nodes

node_values = [nodes[i] for i in range(12,26) if i in nodes.keys()]

plt.boxplot(node_values)
plt.title('Results for m=16')
plt.xlabel('# of Nodes')
plt.ylabel('Time to Schedule')
plt.xticks([i for i in range(1,15)], [i for i in range(12,26)])
plt.show()

# fig, axes = plt.subplots(nrows=1, ncols=2, sharey=True)

# meanpointprops = dict(marker='D', markeredgecolor='black',
#                       markerfacecolor='firebrick')

# axes[0, 0].boxplot(np.array([[1,2,3,4],[3,6,3,7]]), meanprops=meanpointprops, meanline=False,
#                    showmeans=True)
# axes[0, 0].set_title('Default', fontsize=fs)

# axes[0, 1].boxplot([[1,2,3,4],[3,6,3,7]], meanprops=meanpointprops, meanline=False,
#                    showmeans=True)
# axes[0, 1].set_title('Default', fontsize=fs)

# for ax in axes.flatten():
#     ax.set_yscale('log')
#     ax.set_yticklabels([])

# fig.subplots_adjust(hspace=0.4)
# plt.title('Results')
# plt.show()

