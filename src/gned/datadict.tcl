#==========================================================================
#  DATADICT.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   By Andras Varga
#
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


#------------------------------------------------
# Data structure:
#
#  ned(key,fieldname) array.
#
#  keys are: knnn
#  the ned(nextkey) variable stores the next available key number
#
#  fieldnames have some conventions
#    *-cid             object's component on the canvas
#    *-name            object is linked to the object
#
#  type-selection fieldnames (indentation reflects "contains" hierarchy)
#    nedfile
#      import
#      channel
#        chanattr
#      network
#      module
#        machines
#          machine
#        gates
#          gate
#        params
#          param
#        submod
#          substmachines
#            substmachine
#          substparams
#            substparam
#          gatesizes
#            gatesize
#        conn
#          chanattr
#        forloop
#          loopvar
#          conn
#            chanattr
#------------------------------------------------


#
# common attributes: present in all items
#
set ddict(common,type)            {}
set ddict(common,parentkey)       {}
set ddict(common,childrenkeys)    {}
set ddict(common,ownedkeys)       {}
set ddict(common,order)           {}

#
# root
#
set ddesc(root,parents)           {}
set ddesc(root,treeicon)          {cogwheel_vs}
set ddict(root,type)              {root}

#
# nedfile
#
set ddesc(nedfile,plusindent)      {}
set ddesc(nedfile,parents)         {root}
set ddesc(nedfile,treeicon)        {file_vs}
set ddict(nedfile,type)            {nedfile}
set ddict(nedfile,name)            {untitled.ned}
set ddict(nedfile,filename)        {untitled.ned}
set ddict(nedfile,banner-comment)  {}

#
# imports
#
set ddesc(imports,plusindent)       {}
set ddesc(imports,parents)          {nedfile}
set ddesc(imports,treeicon)         {incl_vs}
set ddict(imports,type)             {imports}
set ddict(imports,banner-comment)   {}
set ddict(imports,right-comment)    {}
set ddict(imports,trailing-comment) {}

#
# import
#
set ddesc(import,plusindent)       {    }
set ddesc(import,parents)          {imports}
set ddict(import,type)             {import}
set ddict(import,name)             {}
set ddict(import,banner-comment)   {}
set ddict(import,right-comment)    {}

#
# channel
#
set ddesc(channel,plusindent)       {}
set ddesc(channel,parents)          {nedfile}
set ddesc(channel,treeicon)         {chan_vs}
set ddict(channel,type)             {channel}
set ddict(channel,name)             {Channel}
set ddict(channel,banner-comment)   {}
set ddict(channel,right-comment)    {}
set ddict(channel,trailing-comment) {}

#
# chanattr (channel attribute: delay, error or datarate)
#
set ddesc(chanattr,plusindent)       {    }
set ddesc(chanattr,parents)          {channel}
set ddict(chanattr,type)             {chanattr}
set ddict(chanattr,banner-comment)   {}
set ddict(chanattr,right-comment)    {}
set ddict(chanattr,name)             {}
set ddict(chanattr,value)            {}

#
# network
#
set ddesc(network,plusindent)       {}
set ddesc(network,parents)          {nedfile}
set ddesc(network,treeicon)         {net_vs}
set ddict(network,type)             {network}
set ddict(network,name)             {Network}
set ddict(network,banner-comment)   {}
set ddict(network,right-comment)    {}
set ddict(network,trailing-comment) {}
set ddict(network,type-name)        {Module}
set ddict(network,like-name)        {}

#
# simple
#
set ddesc(simple,plusindent)       {}
set ddesc(simple,parents)          {nedfile}
set ddesc(simple,treeicon)         {simple_vs}
set ddict(simple,type)             {simple}
set ddict(simple,name)             {Simple}
set ddict(simple,banner-comment)   {}
set ddict(simple,right-comment)    {}
set ddict(simple,trailing-comment) {}

#
# module
#
set ddesc(module,plusindent)       {}
set ddesc(module,parents)          {nedfile}
set ddesc(module,treeicon)         {compound_vs}
set ddict(module,type)             {module}
set ddict(module,name)             {Module}
set ddict(module,banner-comment)   {}
set ddict(module,right-comment)    {}
set ddict(module,trailing-comment) {}
set ddict(module,rect-cid)         {}
set ddict(module,rect2-cid)        {}
set ddict(module,label-cid)        {}
set ddict(module,background-cid)   {}
set ddict(module,x-pos)            {}
set ddict(module,y-pos)            {}
set ddict(module,x-size)           {}
set ddict(module,y-size)           {}
set ddict(module,outline-color)    {}
set ddict(module,fill-color)       {}
set ddict(module,linethickness)    {}
set ddict(module,selected)         {0}

#
# params
#
set ddesc(params,plusindent)       {    }
set ddesc(params,parents)          {simple module}
set ddict(params,type)             {params}
set ddict(params,banner-comment)   {}
set ddict(params,right-comment)    {}

#
# param
#
set ddesc(param,plusindent)       {    }
set ddesc(param,parents)          {params}
set ddict(param,type)             {param}
set ddict(param,banner-comment)   {}
set ddict(param,right-comment)    {}
set ddict(param,name)             {param}
set ddict(param,datatype)         {numeric}

#
# gates
#
set ddesc(gates,plusindent)       {    }
set ddesc(gates,parents)          {simple module}
set ddict(gates,type)             {gates}
set ddict(gates,banner-comment)   {}
set ddict(gates,right-comment)    {}

#
# gate
#
set ddesc(gate,plusindent)       {    }
set ddesc(gate,parents)          {gates}
set ddict(gate,type)             {gate}
set ddict(gate,banner-comment)   {}
set ddict(gate,right-comment)    {}
set ddict(gate,name)             {out}
set ddict(gate,gatetype)         {output}
set ddict(gate,isvector)         {0}

#
# machines
#
set ddesc(machines,plusindent)       {    }
set ddesc(machines,parents)          {simple module}
set ddict(machines,type)             {machines}
set ddict(machines,banner-comment)   {}
set ddict(machines,right-comment)    {}

#
# machine
#
set ddesc(machine,plusindent)       {    }
set ddesc(machine,parents)          {machines}
set ddict(machine,type)             {machine}
set ddict(machine,banner-comment)   {}
set ddict(machine,right-comment)    {}
set ddict(machine,name)             {machine}

#
# submods
#
set ddesc(submods,plusindent)       {    }
set ddesc(submods,parents)          {module}
set ddict(submods,type)             {submods}
set ddict(submods,banner-comment)   {}
set ddict(submods,right-comment)    {}

#
# submod
#
set ddesc(submod,plusindent)       {    }
set ddesc(submod,parents)          {submods}
set ddict(submod,type)             {submod}
set ddict(submod,name)             {submod}
set ddict(submod,banner-comment)   {}
set ddict(submod,right-comment)    {}
set ddict(submod,type-name)        {Module}
set ddict(submod,vectorsize)       {}
set ddict(submod,like-name)        {}
set ddict(submod,rect-cid)         {}
set ddict(submod,icon-cid)         {}
set ddict(submod,label-cid)        {}
set ddict(submod,icon)             {}
set ddict(submod,x-pos)            {}
set ddict(submod,y-pos)            {}
set ddict(submod,x-size)           {}
set ddict(submod,y-size)           {}
set ddict(submod,outline-color)    {}
set ddict(submod,fill-color)       {}
set ddict(submod,linethickness)    {}
set ddict(submod,selected)         {0}

#
# substparams
#
set ddesc(substparams,plusindent)       {    }
set ddesc(substparams,parents)          {submod}
set ddict(substparams,type)             {substparams}
set ddict(substparams,banner-comment)   {}
set ddict(substparams,right-comment)    {}
set ddict(substparams,condition)        {}

#
# substparam
#
set ddesc(substparam,plusindent)       {    }
set ddesc(substparam,parents)          {substparams}
set ddict(substparam,type)             {substparam}
set ddict(substparam,banner-comment)   {}
set ddict(substparam,right-comment)    {}
set ddict(substparam,name)             {}
set ddict(substparam,value)            {}

#
# gatesizes
#
set ddesc(gatesizes,plusindent)       {    }
set ddesc(gatesizes,parents)          {submod}
set ddict(gatesizes,type)             {gatesizes}
set ddict(gatesizes,banner-comment)   {}
set ddict(gatesizes,right-comment)    {}
set ddict(gatesizes,condition)        {}

#
# gatesize
#
set ddesc(gatesize,plusindent)       {    }
set ddesc(gatesize,parents)          {gatesizes}
set ddict(gatesize,type)             {gatesize}
set ddict(gatesize,banner-comment)   {}
set ddict(gatesize,right-comment)    {}
set ddict(gatesize,name)             {}
set ddict(gatesize,size)             {}

#
# substmachines
#
set ddesc(substmachines,plusindent)       {    }
set ddesc(substmachines,parents)          {submod}
set ddict(substmachines,type)             {substmachines}
set ddict(substmachines,banner-comment)   {}
set ddict(substmachines,right-comment)    {}
set ddict(substmachines,condition)        {}

#
# substmachine
#
set ddesc(substmachine,plusindent)       {    }
set ddesc(substmachine,parents)          {substmachines}
set ddict(substmachine,type)             {substmachine}
set ddict(substmachine,banner-comment)   {}
set ddict(substmachine,right-comment)    {}
set ddict(substmachine,value)            {}

#
# conns
#
set ddesc(conns,plusindent)       {    }
set ddesc(conns,parents)          {module}
set ddict(conns,type)             {conns}
set ddict(conns,banner-comment)   {}
set ddict(conns,right-comment)    {}
set ddict(conns,nocheck)          {0}

#
# conn
#
set ddesc(conn,plusindent)      {    }
set ddesc(conn,parents)         {conns forloop}
set ddict(conn,type)            {conn}
set ddict(conn,banner-comment)  {}
set ddict(conn,right-comment)   {}
set ddict(conn,src_index)       {}
set ddict(conn,dest_index)      {}
set ddict(conn,srcgate)         {out}
set ddict(conn,destgate)        {in}
set ddict(conn,src_gate_index)  {}
set ddict(conn,dest_gate_index) {}
set ddict(conn,condition)       {}
set ddict(conn,arrowdir-l2r)    {1}
set ddict(conn,arrow-cid)       {}
set ddict(conn,fill-color)      {}
set ddict(conn,linethickness)   {}
set ddict(conn,drawmode)        {}
set ddict(conn,an_src_x)        {}
set ddict(conn,an_src_y)        {}
set ddict(conn,an_dest_x)       {}
set ddict(conn,an_dest_y)       {}
set ddict(conn,selected)        {0}

#
# connattr (connection attribute: channel, delay, error or datarate)
#
set ddesc(connattr,plusindent)       {    }
set ddesc(connattr,parents)          {conn}
set ddict(connattr,type)             {connattr}
set ddict(connattr,banner-comment)   {}
set ddict(connattr,right-comment)    {}
set ddict(connattr,name)             {}
set ddict(connattr,value)            {}

#
# forloop
#
set ddesc(forloop,plusindent)       {    }
set ddesc(forloop,parents)          {conns}
set ddict(forloop,type)             {forloop}
set ddict(forloop,banner-comment)   {}
set ddict(forloop,right-comment)    {}
set ddict(forloop,trailing-comment) {}

#
# loopvar
#
set ddesc(loopvar,plusindent)       {    }
set ddesc(loopvar,parents)          {forloop}
set ddict(loopvar,type)             {loopvar}
set ddict(loopvar,banner-comment)   {}
set ddict(loopvar,right-comment)    {}
set ddict(loopvar,name)          {}
set ddict(loopvar,fromvalue)        {}
set ddict(loopvar,tovalue)          {}


