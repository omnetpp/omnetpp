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

# initially enabled/disabled the omnetpp pretty printers
omnetpp_pp_enabled = True

##############################################

omnetpp_printer_debug = False #or True
def debug(s):
    global omnetpp_printer_debug
    if omnetpp_printer_debug:
        print(s)


##############################################

class cPrinterBase:
    "Base class for OMNeT++ pretty printers"

    def __init__ (self, val):
        self.val = val
        self.addr = long(self.val.address)
        if (self.addr < 0x1000):
            self.addr = 0

    def to_string(self):
        return None

    @staticmethod
    def cast_pointer_to_array(pointer, length):
        debug("cast_pointer_to_array: %s[%d]" % (pointer.dereference().type.tag, length))
        if length > 0:
            return pointer.cast(pointer.dereference().type.array(length-1).pointer()).dereference()
        return pointer

    def xchildren(self):
        for fld in self.val.type.fields():
            try:
                if fld.is_base_class:
                    yield(fld.name, self.val.cast(fld.type))
                else:
                    yield(fld.name, self.val[fld.name])
            except Exception as e:
                print('<ERROR %s>' % e)
                yield(fld.name, '<ERROR %s>' % e)


##############################################

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


##############################################

class cObjectPrinter(cPrinterBase):
    "Print a cObject"

    def __init__ (self, val):
        cPrinterBase.__init__(self, val)

    def getFullPath(self):
        global gdb
        if (self.addr == 0):
            return None
        try:
            path = gdb.parse_and_eval("((::cObject *)%s)->getFullPath()" % str(self.addr))
        except Exception as e:
            print('Error in getFullPath: %s' % e)
            path = None
        return path

    def getFullName(self):
        global gdb
        if (self.addr == 0):
            return None
        try:
            name = gdb.parse_and_eval("((::cObject *)%s)->getFullName()" % str(self.addr))
        except Exception as e:
            print('Error in getFullName: %s' % e)
            name = None
        return name

    def getOwner(self):
        global gdb
        if (self.addr == 0):
            return None
        try:
            owner = gdb.parse_and_eval("((::cObject *)%s)->getOwner()" % str(self.addr))
        except Exception as e:
            print('Error in getOwner: %s' % e)
            owner = None
        return owner

    def getInfo(self):
        global gdb
        if (self.addr == 0):
            return None
        try:
            info = gdb.parse_and_eval("((::cObject *)%s)->info()" % str(self.addr))
        except Exception as e:
            print('Error in getInfo: %s' % e)
            info = None
        return info

    def to_string(self):
        if (self.addr == 0):
            return 'addr=0x0'
        name = self.getFullName()
        path = self.getFullPath()
        #owner = self.getOwner()
        info = self.getInfo()
        ret = 'type "%s"' % self.val.dynamic_type.tag
        if path:
            ret = ret + ' path %s' % path
        elif name:
            ret = ret + ' name %s' % name
        if info:
            ret = ret + ' info %s' % info
        return ret


##############################################

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
        global omnetpp_pp_enabled
        super(OppSubPrinter, self).__init__()
        self.name = name
        self.function = function
        self.enabled = omnetpp_pp_enabled

    def invoke(self, value):
        if not self.enabled:
            return None
        return self.function(value)

# A pretty-printer that conforms to the "PrettyPrinter" protocol from
# gdb.printing.  It can also be used directly as an old-style printer.
class OppPrinter(object):
    def __init__(self, name):
        global omnetpp_pp_enabled
        super(OppPrinter, self).__init__()
        self.name = name
        self.subprinters = []   # used by 'gdb command: info pretty-printer'
        self.lookup = {}
        self.enabled = omnetpp_pp_enabled

    def add(self, name, function):
        printer = OppSubPrinter(name, function)
        self.subprinters.append(printer)
        self.lookup[name] = printer

    @staticmethod
    def get_basic_type(type):
        # If it points to a reference, get the reference.
        #if type.code == gdb.TYPE_CODE_REF:
        if type.code == gdb.TYPE_CODE_REF or type.code == gdb.TYPE_CODE_PTR:
            type = type.target()

        # Get the unqualified type, stripped of typedefs.
        type = type.unqualified().strip_typedefs()

        return type.tag

    def __call__(self, val):
        typename = self.get_basic_type(val.type)
        debug("BASIC TYPE OF '%s' type IS '%s'" % (val.type, typename))
        if not typename:
            return None

        debug(" lookup printer for '%s' type" % (typename))
        if typename in self.lookup:
            if val.type.code == gdb.TYPE_CODE_REF or val.type.code == gdb.TYPE_CODE_PTR:
                debug("---pointer--- %d" % long(val))
                if (long(val) == 0):
                    return None
                val = val.dereference()
            debug(" +printer found for '%s' type" % (typename))
            return self.lookup[typename].invoke(val)

        # Cannot find a pretty printer.  Return None.
        debug(" -printer not found for '%s' type" % (typename))
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
# As of GDB 7.5 and CDT 3.8.1, GDB often crashes during pretty printing
# a cObject pointer so the cObject pretty printer routines are disabled for now.
#    omnetpp_printer.add('cObject', cObjectPrinter)

build_omnetpp_dictionary()

# end

