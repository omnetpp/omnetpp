import sys
import os
import gdb
import tempfile

# set unwind signal so an exeption caused by the pretty printer will not stop the debugger
gdb.execute('set unwindonsignal on')

# skip standard C++ headers when stepping
global skippedstdcxxheaders
skippedstdcxxheaders = False

def skipAllIn(root):
    import os
    for root, dirs, files in os.walk(root, topdown=False):
        for name in files:
            path = os.path.join(root, name)
            gdb.execute('skip file %s' % path, to_string=True)

gdb.execute('source %s' % os.path.join(os.path.dirname(__file__), 'gdbinit'))

# add the pretty printer classes to the system class path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# register libstdc++ pretty printers
if 'register_libstdcxx_printers' in dir():
    print('libstdc++ pretty printers already initialized.')
else:
    from libstdcxx.v6.printers import register_libstdcxx_printers
    register_libstdcxx_printers(None)
    print('Pretty printers initialized: libstdc++')

# register OMNeT++-specific pretty printers
if 'register_omnetpp_printers' in dir():
    print('omnetpp pretty printers already initialized.')
else:
    from omnetpp.printers import register_omnetpp_printers
    register_omnetpp_printers(None)
    print('Pretty printers initialized: omnetpp')

# source the temporary init file written out by the IDE (if exists)
# this file is used to include additional project-specific init scripts

initfile = tempfile.gettempdir() + '/gdbinit.tmp'
if os.path.isfile(initfile):
    exec(open(initfile).read())

