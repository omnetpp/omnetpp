from matplotlib.widgets import Cursor
import pylab

fig = pylab.figure(figsize=(8,6))
ax = fig.add_axes([0.075, 0.25, 0.9, 0.725], axisbg='#FFFFCC')

x,y = 4*(pylab.rand(2,100)-.5)
ax.plot(x,y,'o')
ax.set_xlim(-2,2)
ax.set_ylim(-2,2)

cursor = Cursor(ax, useblit=False, color='red', linewidth=2 )

pylab.show()

