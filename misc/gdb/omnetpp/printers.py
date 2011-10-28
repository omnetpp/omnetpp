#
# Pretty-printers for OMNeT++ classes.
#
# Copyright (C) 2011 OpenSim Ltd.
#
# This file is distributed WITHOUT ANY WARRANTY. See the file
# `license' for details on this and other legal matters.
#

import gdb
from decimal import *
import pprint

# Try to use the new-style pretty-printing if available.
_use_gdb_pp = True
try:
    import gdb.printing
except ImportError:
    _use_gdb_pp = False


class SimTimePrinter:
    "Print a SimTime"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        if (self.val['t'] == 0):
            return "0s"
        s = Decimal(str(self.val['t']) + 'E' + str(self.val['scaleexp']))
        s = s.normalize()
        return str(s) + "s"


class cObjectPrinter:
    "Print a cObject"

    def __init__ (self, val):
        self.val = val

    def children(self):
        global gdb
        val = ""
        addr = long(self.val.address)
        if (addr == 0):
            return

        str = "((::cObject *)0x%x)->" % (addr)

        name = gdb.parse_and_eval(str + "getFullName()")
        path = gdb.parse_and_eval(str + "getFullPath()")

        yield('name', name)
        yield('path', path)

    def to_string (self):
        return None


class cNamedObjectPrinter(cObjectPrinter):
    "Print a cNamedObject"

    def __init__(self, name):
        cObjectPrinter.__init__(self, name)


class cOwnedObjectPrinter(cNamedObjectPrinter):
    "Print a cOwnedObject/cNoncopyableOwnedObject"

    def __init__(self, name):
        cNamedObjectPrinter.__init__(self, name)

    def children(self):
        for x in cNamedObjectPrinter.children(self):
            yield(x)
        yield('owner', self.val['ownerp'])
        yield('pos', self.val['pos'])


class cGatePrinter(cObjectPrinter):
    "Print a cGate"

    def __init__(self, name):
        cObjectPrinter.__init__(self, name)

    def children(self):
        for x in cObjectPrinter.children(self):
            yield(x)
        pos = int(self.val['pos'])
        yield('type', 'output' if (pos&1) else 'input')
        yield('deliver', 'start' if (pos&2) else 'end')
        if (pos >= 0):
            yield('pos', pos >> 2)


class typeinfoPrinter:
    "Print the type of a value"

    def __init__ (self, val):
        self.val = val

    def to_string (self):
        return "type: "+str(self.val.type)

#########################################################################################

# A "regular expression" printer which conforms to the
# "SubPrettyPrinter" protocol from gdb.printing.
class OppSubPrinter(object):
    def __init__(self, name, function):
        super(OppSubPrinter, self).__init__()
        self.name = name
        self.function = function
        self.enabled = True

    def invoke(self, value):
        if not self.enabled:
            return None
        return self.function(value)

# A pretty-printer that conforms to the "PrettyPrinter" protocol from
# gdb.printing.  It can also be used directly as an old-style printer.
class OppPrinter(object):
    def __init__(self, name):
        super(OppPrinter, self).__init__()
        self.name = name
        self.lookup = {}
        self.enabled = True

    def add(self, name, function):
        printer = OppSubPrinter(name, function)
        self.lookup[name] = printer

    @staticmethod
    def get_basic_type(type):
        # If it points to a reference, get the reference.
        if type.code == gdb.TYPE_CODE_REF or type.code == gdb.TYPE_CODE_PTR:
            type = type.target()

        # Get the unqualified type, stripped of typedefs.
        type = type.unqualified().strip_typedefs()

        return type.tag

    def __call__(self, val):
        typename = self.get_basic_type(val.type)
        #print "BASIC TYPE OF '%s' type IS '%s'" % (val.type, typename)
        if not typename:
            return None

        #print "lookup printer for '%s' type" % (typename)
        if typename in self.lookup:
            if val.type.code == gdb.TYPE_CODE_REF or val.type.code == gdb.TYPE_CODE_PTR:
                if (long(val) == 0):
                    return None
                val = val.dereference()
            return self.lookup[typename].invoke(val)

        # Cannot find a pretty printer.  Return None.
        return None


omnetpp_printer = None


def register_omnetpp_printers(obj):
    "Register OMNeT++ pretty-printers with objfile Obj."

    global _use_gdb_pp
    global omnetpp_printer

    if _use_gdb_pp:
        gdb.printing.register_pretty_printer(obj, omnetpp_printer)
    else:
        if obj is None:
            obj = gdb
        obj.pretty_printers.append(omnetpp_printer)


def build_omnetpp_dictionary():
    global omnetpp_printer

    omnetpp_printer = OppPrinter("omnetpp")

    omnetpp_printer.add('SimTime', SimTimePrinter)
    omnetpp_printer.add('cObject', cObjectPrinter)
    omnetpp_printer.add('cNamedObject', cNamedObjectPrinter)
    omnetpp_printer.add('cOwnedObject', cOwnedObjectPrinter)
    omnetpp_printer.add('cNoncopyableOwnedObject', cOwnedObjectPrinter)
    omnetpp_printer.add('cGate', cGatePrinter)


build_omnetpp_dictionary()

# end
