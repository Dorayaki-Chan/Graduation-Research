import os
import csv
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import tkinter as tk
from tkinter import filedialog

# Select the folder containing CSV files
root = tk.Tk()
root.withdraw()
iDir = os.path.abspath(os.path.dirname(__file__))
iDirPath = filedialog.askdirectory(initialdir=iDir)

# Get a list of CSV files in the selected folder
fileList = [f for f in os.listdir(iDirPath) if f.endswith(".csv")]

datas = []
colors = ['b', 'g', 'r', 'c', 'm', 'y', 'k']  # You can add more colors if needed

# Set a smaller plot size
plt.figure(figsize=(10, 10))  # Adjust the width and height as needed

# Read and plot data from each CSV file
for i, filename in enumerate(fileList):
    csvPath = os.path.join(iDirPath, filename)

    # Read CSV file into a DataFrame
    df = pd.read_csv(csvPath)

    # Extract X and Y data
    x = df.iloc[:, 2]  # Assuming the X data is in the third column
    y = df.iloc[:, 3]  # Assuming the Y data is in the fourth column

    # Plot the data with a unique color for each file
    plt.scatter(x, y, label=filename, color=colors[i % len(colors)])

# Set plot limits and labels
plt.xlim(-1500, 0)
plt.ylim(1000, 2500)
plt.xlabel('X-axis')
plt.ylabel('Y-axis')

# Add grid lines and ticks
plt.grid(True)
plt.xticks(np.arange(-1500, 0, 500))
plt.yticks(np.arange(1000, 2501, 500))

# Add legend
plt.legend()



# Save the plot to a file
plt.savefig("scatter_plot.svg")

# Show the plot
plt.show()
