//==========================================================================
//  MAIN.CC - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//  Function main()
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include "cobject.h"     // cStaticFlag
#include "csimul.h"
#include "random.h"
#include "cenvir.h"
#include "onstartup.h"


//
// The main() function
//
ENVIR_API int main(int argc, char *argv[])
{
    cStaticFlag dummy;

    printf("OMNeT++ Discrete Event Simulation  (C) 1992-2003 Andras Varga\n");
    printf("See the license for distribution terms and warranty disclaimer\n");

    if (testrand()==0)
    {
       printf("Warning: Random number generator does not appear to work correctly!");
       while( getc(stdin)!='\n' );
    }

    ev.setup(argc,argv);
    int ret = ev.run();
    ev.shutdown();

    printf("\nEnd run of OMNeT++\n");
    return ret;
}

