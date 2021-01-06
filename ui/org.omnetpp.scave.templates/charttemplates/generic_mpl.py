import numpy as np
import matplotlib.pyplot as plt
from omnetpp.scave import chart, utils

# Take the input string entered in the Properties dialog.
# It is up to the script how to interpret it. In this example, we just print it.
input = chart.get_property("input")
print("Script input: ", input)
props = chart.get_properties()

# Example script -- change at your will
r = np.arange(0, 2, 0.01)
theta = 2 * np.pi * r

ax = plt.subplot(111, projection='polar')
ax.plot(theta, r)
ax.set_rmax(2)
ax.set_rticks([0.5, 1, 1.5, 2])  # Less radial ticks
ax.set_rlabel_position(-22.5)  # Move radial labels away from plotted line
ax.grid(True)

ax.set_title("A line plot on a polar axis", va='bottom')

utils.export_image_if_needed(props)
#utils.export_data_if_needed(df, props)

