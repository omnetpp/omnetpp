//==========================================================================
//   ERRMSG.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Error code constants
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ERRMSG_H
#define __ERRMSG_H


//
// Error codes
//
enum {
        eOK          =  0,
        eBADCAST     =  1,
        eCIRCREF     =  2,
        eNUMARGS     =  3,
        eNOMODIF     =  4,
        eUISTARTUP   =  5,
        eSTOPSIMRCVD =  6,
        eNOMOD       =  7,
        eNOGATE      =  8,
        eINGATE      =  9,
        eNOPARAM     = 10,
        eNOMEM       = 11,
        eINCAUSAL    = 12,
        eMODFIN      = 13,
        eBADTRANSF   = 14,
        eBADINIT     = 15,
        eUNEXP       = 16,
        eNORECV      = 17,
        eSTKLOW      = 18,
        eENDEDOK     = 19,
        eMODINI      = 20,
        eNULLPTR     = 21,
        eNULLREF     = 22,
        eOUTVECT     = 23,
        ePARCHF      = 24,
        eNEGTIME     = 25,
        eMATH        = 26,
        eFPERR       = 27,
        eNEGTOUT     = 28,
        eNODEL       = 29,
        eCANCEL      = 30,
        eNONET       = 31,
        eBADEXP      = 32,
        eFINISH      = 33,
        eENDSIM      = 34,
        eREALTIME    = 35,
        eSIMTIME     = 36,
        eCANTDUP     = 37,
        eINFLOOP     = 38,
        eSTATECHG    = 39,
        eCANTCOPY    = 40,
        eBADGATE     = 41,
        eNOTCONN     = 42,
        eBADKEY      = 43,
        eNOMODDEF    = 44,
        eNOCHANDEF   = 45,
        eNOSUCHNET   = 46,
        ePVM         = 47,
        eBADARGS     = 48,
        eBACKSCHED   = 49,
        eOUTGATE     = 50,
        eCHERROR     = 51, //NL
        eCHDELAY     = 52, //NL
        eARRAY       = 53, //NL
        eCUSTOM      = 54,
        eNOMEM2      = 55,
        eUSER        = 56
};

extern char *emsg[];

#endif
