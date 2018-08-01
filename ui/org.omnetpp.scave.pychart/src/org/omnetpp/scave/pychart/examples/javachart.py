import omnetpp as opp
import numpy as np
import pandas as pd

vectors = opp.getVectors('name(serverChannelState:vector)')
vectors = vectors[vectors.type == "vector"]

#for t in vectors[["vectime", "vecvalue"]].itertuples(index=False):
#    opp.plotVector(t[0], t[1]*3)

v = vectors[["vectime", "vecvalue"]].iloc[0]
opp.plotVectors([{"xs": v["vectime"], "ys": v["vecvalue"]*3}])

print(vectors.head())
# xs, xy = np.array([1.2,2,3.0]), np.array([4,6.0,5])

N = 100000
xs = np.arange(N)
ys = np.random.randn(N)

#opp.plotVector(xs, ys)