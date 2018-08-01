import pandas as pd
import prettyplotlib as ppl
import time

startTime = time.perf_counter()
#vectors = opp.getVectors('*')
#vectors = opp.getVectors('module("Aloha.server")')
vectors = opp.getVectors('name(serverChannelState:vector)')

endTime = time.perf_counter()

print("getVectors took " + str((endTime - startTime) * 1000.0) + " ms")

print(str(len(vectors)))

df = vectors[vectors.type == "vector"]

print(str(df.head()))


startTime = time.perf_counter()
for t in df[["vectime", "vecvalue"]].itertuples(index=False):
    plt.plot(t[0], t[1], drawstyle='steps-post')

endTime = time.perf_counter()
print("plotting took " + str((endTime - startTime) * 1000.0) + " ms")

#plt.tight_layout()

#plt.show()