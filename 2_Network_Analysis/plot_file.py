import pandas as pd
import matplotlib.pyplot as plt

# Load the congestion window log data
data = pd.read_csv('cwnd_log.txt', delim_whitespace=True, header=None)
data.columns = ['Time', 'CongestionWindow']

# Plot the congestion window over time
plt.figure(figsize=(10, 5))
plt.plot(data['Time'], data['CongestionWindow'], label='Congestion Window', color='blue')
plt.title('Congestion Window vs Time')
plt.xlabel('Time (s)')
plt.ylabel('Congestion Window (bytes)')
plt.grid()
plt.legend()
plt.savefig('cwnd_plot.png')  # Save the plot as an image
plt.show()  # Display the plot
