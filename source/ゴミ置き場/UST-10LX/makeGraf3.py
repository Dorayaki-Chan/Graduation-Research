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
plt.figure(figsize=(8, 6))  # Adjust the width and height as needed

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
plt.xlim(-1500, 1500)
plt.ylim(0, 4000)
plt.xlabel('X-axis')
plt.ylabel('Y-axis')

# Add grid lines and ticks
plt.grid(True)
plt.xticks(np.arange(-1500, 1501, 1000))
plt.yticks(np.arange(0, 4001, 1000))

# Add legend
plt.legend()

# Add data values when hovering the cursor
def on_hover(event):
    for i, filename in enumerate(fileList):
        contains, index = plt.gca().contains(event)
        if contains:
            ind = index.get('ind', [None])[0]  # Use get method to provide a default value
            if ind is not None:
                x_val = x.iloc[ind]
                y_val = y.iloc[ind]
                plt.gca().set_title(f'{filename}: X={x_val}, Y={y_val}')

plt.connect('motion_notify_event', on_hover)

# Save the plot to a file
plt.savefig("scatter_plot.png")

# Show the plot
plt.show()
