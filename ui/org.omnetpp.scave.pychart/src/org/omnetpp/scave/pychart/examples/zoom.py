from matplotlib.pyplot import figure, show
import numpy
figsrc = figure()
figzoom = figure()


axsrc = figsrc.add_subplot(111, xlim=(0, 1), ylim=(0, 1), autoscale_on=False)
axzoom = figzoom.add_subplot(111, xlim=(0.45, 0.55), ylim=(0.4, .6),
                             autoscale_on=False)
axsrc.set_title('Move mouse to zoom')
axzoom.set_title('zoom window')
x, y, s, c = numpy.random.rand(4, 200)
s *= 200


axsrc.scatter(x, y, s, c)
axzoom.scatter(x, y, s, c)


def onpress(event):
    #if event.button != 1:
    #    return
    x, y = event.xdata, event.ydata
    if x is None or y is None:
        return
    axzoom.set_xlim(x - 0.1, x + 0.1)
    axzoom.set_ylim(y - 0.1, y + 0.1)
    figzoom.canvas.draw()

figsrc.canvas.mpl_connect('motion_notify_event', onpress)
show()

