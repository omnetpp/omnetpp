//-------------------------------------------------------------
// file: dyna.h
//        (part of DYNA - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "omnetpp.h"

// message kind values (packet types):
enum {
       CONN_REQ,
       CONN_ACK,
       DATA_QUERY,
       DATA_RESULT,
       DISC_REQ,
       DISC_ACK
};
