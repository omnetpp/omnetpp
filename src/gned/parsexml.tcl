#==========================================================================
#  PARSEXML.TCL -
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


#
# This file is *very* experimental at the moment...
#

proc tryParseXML {xml} {

    # should be outside...
    package require xml 1.4

    set p [xml::parser x -elementstartcommand sax_elementstart \
                         -elementendcommand sax_elementend \
                         -characterdatacommand sax_characterdata \
                         -processinginstructioncommand sax_processinginstruction \
                         -externalentityrefcommand sax_externalentityref \
                         -errorcommand sax_error \
                         -warningcommand sax_warning ]
    $p parse $xml
}


proc sax_elementstart args {
  puts "elementstart $args"
  return 0
}

proc sax_elementend args {
  puts "elementend $args"
  return 0
}

proc sax_characterdata args {
  puts "characterdata $args"
  return 0
}

proc sax_processinginstruction args {
  puts "processinginstruction $args"
  return 0
}

proc sax_externalentityref args {
  puts "externalentityref $args"
  return 0
}

proc sax_error args {
  puts "error $args"
  return 0
}

proc sax_warning args {
  puts "warning $args"
  return 0
}



