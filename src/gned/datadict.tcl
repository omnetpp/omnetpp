#=================================================================
#  DATADICT.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2005 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#------------------------------------------------
# Data structure:
#
#  All NED data are stored in a single array: $ned(key,fieldname).
#
#  Keys are unique integer numbers. The $ned(nextkey) variable stores the
#  next available key number.
#
#  the fieldnames follow certainv naming conventions:
#    *-key    refers to another element
#    disp-*   holds display string data
#    aux-*    internally used data
#    *-cid    =canvas id; identifies drawing item (rectangle, line, etc) on the canvas
#
#  type-selection fieldnames (indentation reflects "contains" hierarchy):
#    nedfile
#      import
#      channel
#        chanattr
#      network
#      simple
#      module
#        machines
#          machine
#        gates
#          gate
#        params
#          param
#        submods
#          submod
#            substmachines
#              substmachine
#            substparams
#              substparam
#            gatesizes
#              gatesize
#        conns
#          conn
#            chanattr
#          forloop
#            loopvar
#            conn
#              chanattr
#------------------------------------------------


#
# common attributes: present in all items
#
set ned_attr(common,type)            {}
set ned_attr(common,parentkey)       {}
set ned_attr(common,childrenkeys)    {}
set ned_attr(common,ownedkeys)       {}

#
# root
#
set ned_desc(root,parents)           {}
set ned_desc(root,treeicon)          {cogwheel_vs}


#
# nedfile
#
set ned_desc(nedfile,plusindent)      {}
set ned_desc(nedfile,parents)         {root}
set ned_desc(nedfile,treeicon)        {file_vs}
set ned_attr(nedfile,name)            {Untitled}
set ned_attr(nedfile,filename)        {untitled}
set ned_attr(nedfile,banner-comment)  {}
set ned_attr(nedfile,aux-isunnamed)   {1}
set ned_attr(nedfile,aux-isdirty)     {0}

#
# imports
#
set ned_desc(imports,plusindent)       {}
set ned_desc(imports,parents)          {nedfile}
set ned_desc(imports,treeicon)         {incl_vs}
set ned_attr(imports,banner-comment)   {}
set ned_attr(imports,right-comment)    {}
set ned_attr(imports,trailing-comment) {}

#
# import
#
set ned_desc(import,plusindent)       {    }
set ned_desc(import,parents)          {imports}
set ned_attr(import,name)             {}
set ned_attr(import,banner-comment)   {}
set ned_attr(import,right-comment)    {}

#
# channel
#
set ned_desc(channel,plusindent)       {}
set ned_desc(channel,parents)          {nedfile}
set ned_desc(channel,treeicon)         {chan_vs}
set ned_attr(channel,name)             {Channel}
set ned_attr(channel,banner-comment)   {}
set ned_attr(channel,right-comment)    {}
set ned_attr(channel,trailing-comment) {}

#
# chanattr (channel attribute: delay, error or datarate)
#
set ned_desc(chanattr,plusindent)       {    }
set ned_desc(chanattr,parents)          {channel}
set ned_attr(chanattr,banner-comment)   {}
set ned_attr(chanattr,right-comment)    {}
set ned_attr(chanattr,name)             {}
set ned_attr(chanattr,value)            {}

#
# network
#
set ned_desc(network,plusindent)       {}
set ned_desc(network,parents)          {nedfile}
set ned_desc(network,treeicon)         {net_vs}
set ned_attr(network,name)             {Network}
set ned_attr(network,banner-comment)   {}
set ned_attr(network,right-comment)    {}
set ned_attr(network,trailing-comment) {}
set ned_attr(network,type-name)        {Module}
set ned_attr(network,like-name)        {}

#
# simple
#
set ned_desc(simple,plusindent)       {}
set ned_desc(simple,parents)          {nedfile}
set ned_desc(simple,treeicon)         {simple_vs}
set ned_attr(simple,name)             {Simple}
set ned_attr(simple,banner-comment)   {}
set ned_attr(simple,right-comment)    {}
set ned_attr(simple,trailing-comment) {}

#
# module
#
set ned_desc(module,plusindent)       {}
set ned_desc(module,parents)          {nedfile}
set ned_desc(module,treeicon)         {compound_vs}
set ned_attr(module,name)             {Module}
set ned_attr(module,banner-comment)   {}
set ned_attr(module,right-comment)    {}
set ned_attr(module,trailing-comment) {}
set ned_attr(module,displaystring)    {}
set ned_attr(module,rect-cid)         {}
set ned_attr(module,rect2-cid)        {}
set ned_attr(module,label-cid)        {}
set ned_attr(module,background-cid)   {}
set ned_attr(module,disp-xpos)            {}
set ned_attr(module,disp-ypos)            {}
set ned_attr(module,disp-xsize)           {}
set ned_attr(module,disp-ysize)           {}
set ned_attr(module,disp-outlinecolor)    {}
set ned_attr(module,disp-fillcolor)       {}
set ned_attr(module,disp-linethickness)   {}
set ned_attr(module,aux-isselected)       {0}

#
# params
#
set ned_desc(params,plusindent)       {    }
set ned_desc(params,parents)          {simple module}
set ned_desc(params,treeicon)         {container_vs}
set ned_attr(params,banner-comment)   {}
set ned_attr(params,right-comment)    {}

#
# param
#
set ned_desc(param,plusindent)       {    }
set ned_desc(param,parents)          {params}
set ned_attr(param,banner-comment)   {}
set ned_attr(param,right-comment)    {}
set ned_attr(param,name)             {param}
set ned_attr(param,datatype)         {numeric}

#
# gates
#
set ned_desc(gates,plusindent)       {    }
set ned_desc(gates,parents)          {simple module}
set ned_desc(gates,treeicon)         {container_vs}
set ned_attr(gates,banner-comment)   {}
set ned_attr(gates,right-comment)    {}

#
# gate
#
set ned_desc(gate,plusindent)       {    }
set ned_desc(gate,parents)          {gates}
set ned_attr(gate,banner-comment)   {}
set ned_attr(gate,right-comment)    {}
set ned_attr(gate,name)             {out}
set ned_attr(gate,gatetype)         {out}
set ned_attr(gate,isvector)         {0}

#
# machines
#
set ned_desc(machines,plusindent)       {    }
set ned_desc(machines,parents)          {simple module}
set ned_desc(machines,treeicon)         {container_vs}
set ned_attr(machines,banner-comment)   {}
set ned_attr(machines,right-comment)    {}

#
# machine
#
set ned_desc(machine,plusindent)       {    }
set ned_desc(machine,parents)          {machines}
set ned_attr(machine,banner-comment)   {}
set ned_attr(machine,right-comment)    {}
set ned_attr(machine,name)             {machine}

#
# submods
#
set ned_desc(submods,plusindent)       {    }
set ned_desc(submods,parents)          {module}
set ned_desc(submods,treeicon)         {container_vs}
set ned_attr(submods,banner-comment)   {}
set ned_attr(submods,right-comment)    {}

#
# submod
#
set ned_desc(submod,plusindent)       {    }
set ned_desc(submod,parents)          {submods}
set ned_attr(submod,name)             {submod}
set ned_attr(submod,banner-comment)   {}
set ned_attr(submod,right-comment)    {}
set ned_attr(submod,type-name)        {Unknown}
set ned_attr(submod,vectorsize)       {}
set ned_attr(submod,like-name)        {}
set ned_attr(submod,displaystring)    {}
set ned_attr(submod,rect-cid)         {}
set ned_attr(submod,icon-cid)         {}
set ned_attr(submod,label-cid)        {}
set ned_attr(submod,disp-icon)            {}
set ned_attr(submod,disp-iconcolor)       {}
set ned_attr(submod,disp-iconcolorpc)     {}
set ned_attr(submod,disp-xpos)            {}
set ned_attr(submod,disp-ypos)            {}
set ned_attr(submod,disp-layout)          {}
set ned_attr(submod,disp-layoutpar1)      {}
set ned_attr(submod,disp-layoutpar2)      {}
set ned_attr(submod,disp-layoutpar3)      {}
set ned_attr(submod,disp-xsize)           {}
set ned_attr(submod,disp-ysize)           {}
set ned_attr(submod,disp-shape)           {}
set ned_attr(submod,disp-outlinecolor)    {}
set ned_attr(submod,disp-fillcolor)       {}
set ned_attr(submod,disp-linethickness)   {}
set ned_attr(submod,aux-isselected)       {0}

#
# substparams
#
set ned_desc(substparams,plusindent)       {    }
set ned_desc(substparams,parents)          {submod}
set ned_desc(substparams,treeicon)         {container_vs}
set ned_attr(substparams,banner-comment)   {}
set ned_attr(substparams,right-comment)    {}
set ned_attr(substparams,condition)        {}

#
# substparam
#
set ned_desc(substparam,plusindent)       {    }
set ned_desc(substparam,parents)          {substparams}
set ned_attr(substparam,banner-comment)   {}
set ned_attr(substparam,right-comment)    {}
set ned_attr(substparam,name)             {}
set ned_attr(substparam,value)            {}

#
# gatesizes
#
set ned_desc(gatesizes,plusindent)       {    }
set ned_desc(gatesizes,parents)          {submod}
set ned_desc(gatesizes,treeicon)         {container_vs}
set ned_attr(gatesizes,banner-comment)   {}
set ned_attr(gatesizes,right-comment)    {}
set ned_attr(gatesizes,condition)        {}

#
# gatesize
#
set ned_desc(gatesize,plusindent)       {    }
set ned_desc(gatesize,parents)          {gatesizes}
set ned_attr(gatesize,banner-comment)   {}
set ned_attr(gatesize,right-comment)    {}
set ned_attr(gatesize,name)             {}
set ned_attr(gatesize,size)             {}

#
# substmachines
#
set ned_desc(substmachines,plusindent)       {    }
set ned_desc(substmachines,parents)          {submod}
set ned_desc(substmachines,treeicon)         {container_vs}
set ned_attr(substmachines,banner-comment)   {}
set ned_attr(substmachines,right-comment)    {}
set ned_attr(substmachines,condition)        {}

#
# substmachine
#
set ned_desc(substmachine,plusindent)       {    }
set ned_desc(substmachine,parents)          {substmachines}
set ned_attr(substmachine,banner-comment)   {}
set ned_attr(substmachine,right-comment)    {}
set ned_attr(substmachine,value)            {}

#
# conns
#
set ned_desc(conns,plusindent)       {    }
set ned_desc(conns,parents)          {module}
set ned_desc(conns,treeicon)         {container_vs}
set ned_attr(conns,banner-comment)   {}
set ned_attr(conns,right-comment)    {}
set ned_attr(conns,nocheck)          {0}

#
# conn
#
set ned_desc(conn,plusindent)      {    }
set ned_desc(conn,parents)         {conns forloop}
set ned_attr(conn,banner-comment)  {}
set ned_attr(conn,right-comment)   {}
set ned_attr(conn,src-ownerkey)    {}
set ned_attr(conn,dest-ownerkey)   {}
set ned_attr(conn,src-mod-index)   {}
set ned_attr(conn,dest-mod-index)  {}
set ned_attr(conn,srcgate)         {out}
set ned_attr(conn,destgate)        {in}
set ned_attr(conn,src-gate-index)  {}
set ned_attr(conn,dest-gate-index) {}
set ned_attr(conn,src-gate-plusplus) {0}
set ned_attr(conn,dest-gate-plusplus) {0}
set ned_attr(conn,condition)       {}
set ned_attr(conn,arrowdir-l2r)    {1}
set ned_attr(conn,displaystring)   {}
set ned_attr(conn,arrow-cid)       {}
set ned_attr(conn,disp-fillcolor)     {}
set ned_attr(conn,disp-linethickness) {}
set ned_attr(conn,disp-drawmode)      {}
set ned_attr(conn,disp-src-anchor-x)  {}
set ned_attr(conn,disp-src-anchor-y)  {}
set ned_attr(conn,disp-dest-anchor-x) {}
set ned_attr(conn,disp-dest-anchor-y) {}
set ned_attr(conn,aux-isselected)     {0}

#
# connattr (connection attribute: channel, delay, error or datarate)
#
set ned_desc(connattr,plusindent)       {    }
set ned_desc(connattr,parents)          {conn}
set ned_attr(connattr,banner-comment)   {}
set ned_attr(connattr,right-comment)    {}
set ned_attr(connattr,name)             {}
set ned_attr(connattr,value)            {}

#
# forloop
#
set ned_desc(forloop,plusindent)       {    }
set ned_desc(forloop,parents)          {conns}
set ned_attr(forloop,banner-comment)   {}
set ned_attr(forloop,right-comment)    {}
set ned_attr(forloop,trailing-comment) {}

#
# loopvar
#
set ned_desc(loopvar,plusindent)       {    }
set ned_desc(loopvar,parents)          {forloop}
set ned_attr(loopvar,banner-comment)   {}
set ned_attr(loopvar,right-comment)    {}
set ned_attr(loopvar,name)             {}
set ned_attr(loopvar,fromvalue)        {}
set ned_attr(loopvar,tovalue)          {}

#=====================================================================

#
# Fill ned_attlist() for each type.
#

# collect types
set types {}
foreach i [array names ned_attr] {
    regsub -- ",.*" $i "" type
    if {[lsearch $types $type]==-1} {
        lappend types $type
    }
}

# collect fields for each type
foreach type $types {
    foreach i [array names ned_attr "$type,*"] {
        regsub -- ".*," $i "" field
        lappend ned_attlist($type) $field
    }
}
unset types

#=====================================================================

#
# Init global vars: add a root item with key 0
#
# Note: This must be in the same file as the filling of the ned_attr() array,
# because if Tcl embedding is used (e.g. Tcl code is linked into
# the program as a large C string constant) we cannot control the order
# the Tcl files.
#

foreach i [array names ned_attr "common,*"] {
   regsub -- "common," $i "" field
   set ned(0,$field) $ned_attr($i)
}

foreach i [array names ned_attr "root,*"] {
   regsub -- "root," $i "" field
   set ned(0,$field) $ned_attr($i)
}

set ned(nextkey) 1


