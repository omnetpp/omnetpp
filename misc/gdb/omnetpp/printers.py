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
        self.addr = long(self.val.address)
        if (self.addr < 0x1000):
            self.addr = 0

    def getFullPath(self):
        global gdb
        if (self.addr == 0):
            return '<NULL>'
        try:
            path = gdb.parse_and_eval("((::cObject *)0x%x)->getFullPath()" % (self.addr))
            path = path['_M_dataplus']['_M_p'].string('','replace')
        except Exception as e:
            print 'Error in getFullPath: %s' % e
            path = '<ERROR>'
        return path

    def getFullName(self):
        global gdb
        if (self.addr == 0):
            return '<NULL>'
        try:
            name = gdb.parse_and_eval("((::cObject *)0x%x)->getFullName()" % (self.addr))
            name = name.string('','replace')
        except Exception as e:
            print 'Error in getFullName: %s' % e
            name = '<ERROR>'
        return name

    def getOwner(self):
        global gdb
        if (self.addr == 0):
            return '<NULL>'
        try:
            owner = gdb.parse_and_eval("((::cObject *)0x%x)->getOwner()" % (self.addr))
        except Exception as e:
            print 'Error in getOwner: %s' % e
            owner = 0
        return owner

    def children(self):
        global gdb

        # ### for testing only:
        # print "TYPE='%s'" % self.val.dynamic_type.tag
        # yield('addr', long(self.val.address))
        # yield('type', self.val.type.tag)
        # yield('dyn-type', self.val.dynamic_type.tag)

        if (self.addr == 0):
            return

        name = self.getFullName()
        path = self.getFullPath()
        owner = self.getOwner()

        # ### for testing only
        # yield('name', name)
        # yield('path', path)
        # yield('owner', owner)

        if (name == ''):
            yield('addr', '0x%x' % self.addr)
        else:
            if (long(owner) == 0):
                yield('name', '"%s"' % name)
            else:
                yield('path', '"%s"' % path)

    def to_string (self):
        return None


class cNamedObjectPrinter(cObjectPrinter):
    "Print a cNamedObject"

    def __init__(self, val):
        cObjectPrinter.__init__(self, val)


class cOwnedObjectPrinter(cNamedObjectPrinter):
    "Print a cOwnedObject/cNoncopyableOwnedObject"

    def __init__(self, val):
        cNamedObjectPrinter.__init__(self, val)

    def children(self):
        for x in cNamedObjectPrinter.children(self):
            yield(x)
        #yield('owner', self.val['ownerp'])     # unnecessary, showed in path
        #yield('pos', self.val['pos'])


class cNoncopyableOwnedObjectPrinter(cOwnedObjectPrinter):
    "Print a cNoncopyableOwnedObject"

    def __init__(self, val):
        cOwnedObjectPrinter.__init__(self, val)


class cDefaultListPrinter(cNoncopyableOwnedObjectPrinter):
    "Print a cDefaultList"

    def __init__(self, val):
        cNoncopyableOwnedObjectPrinter.__init__(self, val)


class cComponentPrinter(cDefaultListPrinter):
    "Print a cComponent"

    def __init__(self, val):
        cDefaultListPrinter.__init__(self, val)


class cModulePrinter(cComponentPrinter):
    "Print a cModule"

    def __init__(self, val):
        cComponentPrinter.__init__(self, val)

    def children(self):
        for x in cComponentPrinter.children(self):
            yield(x)
        yield('id', self.val['mod_id'])


class cCompoundModulePrinter(cModulePrinter):
    "Print a cCompoundModule"

    def __init__(self, val):
        cModulePrinter.__init__(self, val)


class cSimpleModulePrinter(cModulePrinter):
    "Print a cSimpleModule"

    def __init__(self, val):
        cModulePrinter.__init__(self, val)


class cParPrinter(cObjectPrinter):
    "Print a cPar"

    def __init__(self, val):
        cObjectPrinter.__init__(self, val)

    def children(self):
        for x in cObjectPrinter.children(self):
            yield(x)
        yield('impl', self.val['p'])


class cParImplPrinter(cNamedObjectPrinter):
    "Print a cParimpl"

    def __init__(self, val):
        cNamedObjectPrinter.__init__(self, val)

    def getType(self):
        global gdb
        if (self.addr == 0):
            return '<NULL>'
        try:
            ret = gdb.parse_and_eval("((::cParImpl *)0x%x)->getType()" % (self.addr))
        except Exception as e:
            print 'Error in getType(): %s' % (e)
            ret = '<ERROR>'
        return ret

    def children(self):
        global gdb
        if (self.addr == 0):
            return

        for x in cNamedObjectPrinter.children(self):
            yield(x)

        if (long(self.val['unitp']) != 0):
            yield('@unit', '"%s"' % (self.val['unitp'].string()))
        type_ = self.getType()
        yield('type', type_)

        flags = long(self.val['flags'])
        if ((flags & 96) == 96):        # FL_CONTAINSVALUE + FL_ISSET
            flaglist = ()
            if (flags & 4): flaglist.append('volatile')
            if (flags & 8): flaglist.append('expr')
            if (len(flaglist)):
                yield('flags', ' '.join(flaglist))


class cGatePrinter(cObjectPrinter):
    "Print a cGate"

    def __init__(self, val):
        cObjectPrinter.__init__(self, val)

    def children(self):
        for x in cObjectPrinter.children(self):
            yield(x)
        pos = int(self.val['pos'])
        yield('type', 'output' if (pos&1) else 'input')
        yield('deliver', 'start' if (pos&2) else 'end')
        if (pos >= 0):
            yield('pos', pos >> 2)


class cQueuePrinter(cOwnedObjectPrinter):
    "Print a cQueue"

    def __init__(self, val):
        cOwnedObjectPrinter.__init__(self, val)

    def children(self):
        for x in cOwnedObjectPrinter.children(self):
            yield(x)
        yield('tkownership', self.val['tkownership'])
        length = long(self.val['n'])
        yield('length', length)
        if (length == 0):
            return

        limit = 5
        skipto = length - limit
        if (skipto <= limit):
            limit = length
            skipto = length
        # print queue items: [0]..[limit-1]   '...'   [skipto] .. [length-1]
        p = self.val['frontp']
        i = 0
        while (long(p) != 0 and i < limit):
            p = p.dereference()
            yield('[%d]' % i, p['obj'])
            p = p['next']
            i += 1
        if (i == length):
            return

        # skip items
        if (skipto <= 2*limit):
            while (long(p) != 0 and i < skipto):
                p = p.dereference()
                p = p['next']
                i += 1
        else:
            p = self.val['backp']
            i = length-1
            while (long(p) != 0 and i > skipto):
                p = p.dereference()
                p = p['prev']
                i -= 1

        if (limit == skipto-1):
            yield('[%d]' % limit, '...')
        else:
            yield('[%d .. %d]' % (limit,skipto-1), '...')

        while (long(p) != 0 and i < length):
            p = p.dereference()
            yield('[%d]' % i, p['obj'])
            p = p['next']
            i += 1


class cChannelPrinter(cComponentPrinter):
    "Print a cModule"

    def __init__(self, val):
        cComponentPrinter.__init__(self, val)

    def callFunc(self, func):
        global gdb
        if (self.addr == 0):
            return '<NULL>'
        try:
            ret = gdb.parse_and_eval("((::cChannel *)0x%x)->%s" % (self.addr, func))
        except Exception as e:
            print 'Error in %s: %s' % (func, e)
            ret = '<ERROR>'
        return ret

    def children(self):
        global gdb
        if (self.addr == 0):
            return
        for x in cComponentPrinter.children(self):
            yield(x)
        srcgate = self.val['srcgatep']
        srcgatename = '<NULL>'
        destgatename = '<NULL>'
        if (long(srcgate) != 0):
            srcgate = srcgate.dereference()
            srcgatename = cGatePrinter(srcgate).getFullPath()
            yield('srcgate', '"%s"' % srcgatename)
            destgate = srcgate['nextgatep']
            if (long(destgate) != 0):
                destgatename = cGatePrinter(destgate.dereference()).getFullPath()
                yield('destgate', '"%s"' % destgatename)

        isTr = self.callFunc("isTransmissionChannel()")
        datarate = self.callFunc("getNominalDatarate()")
        yield('datarate', datarate)
        isBusy = self.callFunc("isBusy()")
        yield('busy', isBusy)
        info = self.callFunc("info()")
        yield('info', info)

        # ### trFinishTime disabled, see GDB BUG 13403: http://sourceware.org/bugzilla/show_bug.cgi?id=13403
        # trFinishTime = self.callFunc("getTransmissionFinishTime()")
        # yield('trFinishTime', trFinishTime)

    def to_string (self):
        return None


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
    omnetpp_printer.add('cNoncopyableOwnedObject', cNoncopyableOwnedObjectPrinter)
    omnetpp_printer.add('cDefaultList', cDefaultListPrinter)
    omnetpp_printer.add('cComponent', cComponentPrinter)
    omnetpp_printer.add('cModule', cModulePrinter)
    omnetpp_printer.add('cCompoundModule', cCompoundModulePrinter)
    omnetpp_printer.add('cSimpleModule', cSimpleModulePrinter)
    omnetpp_printer.add('cPar', cParPrinter)
    omnetpp_printer.add('cParImpl', cParImplPrinter)
    omnetpp_printer.add('cGate', cGatePrinter)
    omnetpp_printer.add('cQueue', cQueuePrinter)
    omnetpp_printer.add('cChannel', cChannelPrinter)


build_omnetpp_dictionary()

# end
