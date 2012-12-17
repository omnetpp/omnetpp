import sys
import os
import gdb
import tempfile

# set unwind signal so an exeption caused by the pretty printer will not stop the debugger
gdb.execute('set unwindonsignal on')
# allow auto-loading of python gdb files from everywhere
gdb.execute('set auto-load safe-path /')

# add the pretty printer classes to the system class path
sys.path.insert(0, os.path.dirname(__file__))

# register libstdc++ pretty printers
if 'register_libstdcxx_printers' in dir():
    print 'libstdc++ pretty printers already initialized.'
else:
    from libstdcxx.v6.printers import register_libstdcxx_printers
    register_libstdcxx_printers(None)
    print 'Pretty printers initialized: libstdc++'

# register omnet specific pretty printers
if 'register_omnetpp_printers' in dir():
    print 'omnetpp pretty printers already initialized.'
else:
    from omnetpp.printers import register_omnetpp_printers
    register_omnetpp_printers(None)
    print 'Pretty printers initialized: omnetpp'

# source the temporary init file written out by the IDE (if exists)
# this file is used to include additional project specific init scripts

initfile = tempfile.gettempdir() + '/initgdb.py'
if os.path.isfile(initfile):
    execfile(initfile)
    os.remove(initfile)
