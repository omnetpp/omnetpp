import math
import numpy as np
import pandas as pd
from omnetpp.scave import results, chart, utils

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

t = np.arange(1, 300)
v1 = np.sin(t / 20)
v2 = np.sin(t / 8)

df = pd.DataFrame({
    "title": ["alpha", "beta"],
    "vectime": [t, t],
    "vecvalue": [v1, v2]
})

# this method works in both native and Matplotlib type charts
utils.plot_vectors(df, props)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
