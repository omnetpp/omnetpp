python

import sys
import os

sys.path.insert(0, os.environ['HOME'] + '/misc/gdb')

if 'register_libstdcxx_printers' in dir():
    print 'libstdc++ pretty printers already initialized.'
else:
    from libstdcxx.v6.printers import register_libstdcxx_printers
    register_libstdcxx_printers (None)
    print 'Pretty printers initialized: libstdc++'

if 'register_omnetpp_printers' in dir():
    print 'omnetpp pretty printers already initialized.'
else:
    from omnetpp.printers import register_omnetpp_printers
    register_omnetpp_printers(None)
    print 'Pretty printers initialized: omnetpp'

end
