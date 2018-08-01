import prettyplotlib as ppl

# prettyplotlib imports
from prettyplotlib import brewer2mpl

# Set the random seed for consistency
np.random.seed(12)

fig, ax = plt.subplots(1)

# Show the whole color range
for i in range(8):
    y = np.random.normal(size=1000).cumsum()
    x = np.arange(1000)

    # For now, you need to specify both x and y :(
    # Still figuring out how to specify just one
    ppl.plot(ax, x, y, label=str(i), linewidth=0.75)

ppl.legend(ax)

