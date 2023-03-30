from omnetpp.scave import scave_bindings

rfm = scave_bindings.ResultFileManager()
rfm.loadFile("Net5SaturatedQueue-#0.sca",
             "../../../samples/resultfiles/routing/Net5SaturatedQueue-#0.sca",
             scave_bindings.LoadFlags.LOADFLAGS_DEFAULTS)

scalars = rfm.getAllScalars()
print(scalars.size())


scalars = rfm.filterIDList(scalars, "name =~ *len*")
print(scalars.size())

print(scalars.asVector())

for i, sid in enumerate(scalars):
    if i >= 10:
        break
    scalar = rfm.getNonfieldScalar(sid)
    print(scalar.getAttributes())
    print(i, scalar.getModuleName(), scalar.getName(), scalar.getValue())



if 0:
    import threading
    rfm = scave_bindings.ResultFileManager()
    intf = scave_bindings.InterruptedFlag()

    t = threading.Thread(target=rfm.loadFile, args=("Net5SaturatedQueue-#0.sca", "../../../samples/resultfiles/routing/Net5SaturatedQueue-#0.sca",
                    scave_bindings.LoadFlags.LOADFLAGS_DEFAULTS, intf))
    t.start()
    import time
    import os
    import numpy # just to pass time
    intf.flag = True
    t.join()
    scalars = rfm.getAllScalars()
    print(scalars.size())




if 0:
    rfm.loadFile("Net5SaturatedQueue-#0.vec",
                "../../../samples/resultfiles/routing/Net5SaturatedQueue-#0.vec",
                scave_bindings.LoadFlags.LOADFLAGS_DEFAULTS)
    vectors = rfm.getAllVectors()
    print(vectors.size())

    vs = scave_bindings.readVectorsIntoArrays(rfm, vectors, False, False)

    print(len(vs))
    print(vs[0].length())


    for i in range(10):
        print(vs[0].getX(i))
        print(vs[0].getY(i))

    vec_x = [vs[0].getX(i) for i in range(vs[0].length())]
    vec_y = [vs[0].getY(i) for i in range(vs[0].length())]


    import matplotlib.pyplot as plt
    plt.plot(vec_x, vec_y)
    plt.show()