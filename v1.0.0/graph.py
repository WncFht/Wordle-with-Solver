import numpy as np
import matplotlib.pyplot as plt
from matplotlib.projections.polar import PolarAxes
from matplotlib.projections import register_projection


# Define colors
COLORS = {
    'blue': '#A7C0DF',
    'pink': '#D5AABE',
    'green': '#8ACB88',
    'purple': '#DCA7EB',
    'yellow': '#E7C217'
}

# Style settings
plt.rcParams['font.size'] = 12
plt.rcParams['axes.titlesize'] = 14
plt.rcParams['axes.labelsize'] = 12
plt.rcParams['legend.fontsize'] = 10

# Data preparation
guesses = np.arange(1, 7)
entropy_dist = [0.04, 0.48, 14.34, 51.58, 28.51, 4.71]
decision_dist = [0.00, 3.37, 52.92, 41.94, 1.77, 0.00]

bar_width = 0.35
opacity = 0.8
index = np.arange(len(guesses))

plt.figure(figsize=(10, 6))

# Create bars
bars1 = plt.bar(index, entropy_dist, bar_width, alpha=opacity, 
                color=COLORS['blue'], label='One-Layer-Entropy')
bars2 = plt.bar(index + bar_width, decision_dist, bar_width, alpha=opacity,
                color=COLORS['pink'], label='Decision-Tree')

# Customize plot
plt.ylim(0, 60)  # Set y-axis limit to slightly above max value
plt.yticks(np.arange(0, 61, 10), [f'{int(t)}' for t in np.arange(0, 61, 10)])

plt.xlabel('Number of Guesses')
plt.ylabel('Percentage of Words (%)')
plt.title('Guess Distribution Comparison')
plt.xticks(index + bar_width / 2, guesses)

# Add legend inside the plot
plt.legend(loc='upper right')

# Add value labels
def add_labels(bars):
    for bar in bars:
        height = bar.get_height()
        if height > 0:  # Only show non-zero values
            plt.text(bar.get_x() + bar.get_width()/2, height,
                    f'{height:.1f}', va='bottom', ha='center')

add_labels(bars1)
add_labels(bars2)

plt.tight_layout()
plt.savefig('guess_distribution.png', format='png', bbox_inches='tight')
plt.show()

# Radar chart
def radar_factory(num_vars):
    theta = np.linspace(0, 2*np.pi, num_vars, endpoint=False)

    class RadarAxes(PolarAxes):
        name = 'radar'
        def __init__(self, *args, **kwargs):
            super().__init__(*args, **kwargs)
            self.set_theta_zero_location('N')
            # self.set_frame_on(False)  # 去掉最外面的框
            # self.xaxis.set_visible(False)  # 隐藏x轴
            # self.yaxis.set_visible(False)  # 隐藏y轴

        def fill(self, *args, **kwargs):
            return super().fill(*args, **kwargs)

        def plot(self, *args, **kwargs):
            return super().plot(*args, **kwargs)

        def set_varlabels(self, labels):
            self.set_thetagrids(np.degrees(theta), labels)

    register_projection(RadarAxes)
    return theta

# Radar chart data
labels = ['Success Rate', 'Average Score', 'Avg Guesses\nEfficiency', 
          'Max Guess\nEfficiency', 'Time\nEfficiency']
entropy_data = [100, 67.6, 57.6, 10, 0.1]
decision_data = [100, 75.8, 65.8, 50, 99.9]

fig = plt.figure(figsize=(10, 8))
theta = radar_factory(len(labels))
ax = fig.add_subplot(111, projection='radar')

ax.plot(theta, entropy_data, 'o-', linewidth=2, 
        color=COLORS['blue'], label='One-Layer-Entropy')
ax.fill(theta, entropy_data, alpha=0.25, color=COLORS['blue'])
ax.plot(theta, decision_data, 'o-', linewidth=2,
        color=COLORS['pink'], label='Decision-Tree')
ax.fill(theta, decision_data, alpha=0.25, color=COLORS['pink'])

ax.set_varlabels(labels)
ax.set_title('Performance Metrics Comparison')
plt.legend(bbox_to_anchor=(1.02, 1), loc='upper left')

# ax.grid(False)
# ax.patch.set_facecolor('white')


plt.tight_layout()
plt.savefig('performance_metrics.png', format='png', bbox_inches='tight')
plt.show()