//==========================================================================
//  SSMIFDEF.H  -  Definitions for SSM I/O interfaces for
//                 Discrete System Simulation in OMNeT++
//
//  Simulated system: TUB Northern&Southern FDDI Rings
//
//  Author: Gabor.Lencse@hit.bme.hu
//==========================================================================

//--------------------------------------------------------------*
//  Copyright (C) 1996,97 Gabor Lencse,
//  Technical University of Budapest, Dept. of Telecommunications,
//  Stoczek u.2, H-1111 Budapest, Hungary.
//
//  This file is distributed WITHOUT ANY WARRANTY. See the file
//  `license' for details on this and other legal matters.
//--------------------------------------------------------------*/

#ifndef __SSMIFDEF_H
#define __SSMIFDEF_H

#define SSM_OIF_HEAPSIZE 32728
#define SSM_IIF_HEAPSIZE 16384

#define SSM_STATISTICS  90 /* SSM statistics from OIF to IIF */
#define SSM_IIF_GENPACK 91 /* SSM IIF message to self */
#define SSM_OIF_SEND    92 /* SSM OIF message to self */
#define SSM_OIF_DELETE  93 /* SSM OIF message to self */

#endif
