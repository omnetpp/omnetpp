import pandas as pd
import prettyplotlib as ppl
import time
from matplotlib.widgets import Cursor

import omnetpp as opp

startTime = time.perf_counter()
##scalars = opp.getScalars("name(busy*)")

#scalars = opp.getScalars('module("Net10.rte[0].queue[0]")')
scalars = opp.getScalars('*')

endTime = time.perf_counter()

print("getScalars took " + str((endTime - startTime) * 1000.0) + " ms")

#print(str(len(scalars)))

df = scalars[scalars.type == "scalar"] # pd.DataFrame(scalars)
df["value"] = pd.to_numeric(df["value"])
df = df[(df.name == "busy:timeavg") & (df.value > 0)]

vals = pd.to_numeric(df["value"]).tolist()
names = df["module"].tolist()

#print(str(list(zip(names, vals))))

fig, ax = plt.subplots(1)

ppl.bar(ax, np.arange(len(vals)), vals)
plt.xticks(np.arange(len(vals)), names, rotation='vertical')

ax.yaxis.grid()

plt.tight_layout()

#cursor = Cursor(ax, useblit=False, color='red', linewidth=2 )


plt.show()