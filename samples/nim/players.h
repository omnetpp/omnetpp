//-------------------------------------------------------------
// file: players.h
//        (part of NIM - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "omnetpp.h"

// The three player types (SimplePlayer,SmartPlayer,InteractivePlayer)
// will be derived from a commom Player base class.

// derive abstract class Player from cSimpleModule
class Player : public cSimpleModule
{
    Module_Class_Members( Player, cSimpleModule, 16384)
       // this is a macro; it expands to constructor definition etc.
       // 16384 is the size for the coroutine stack (in bytes)

    virtual void activity();
       // this redefined virtual function holds the algorithm

    virtual char *playerName() = 0;
    virtual int calculateMove( int num_sticks ) = 0;
};

class SimplePlayer : public Player
{
    Module_Class_Members( SimplePlayer, Player, 16384)
    virtual char *playerName();
    virtual int calculateMove( int num_sticks );
};

class SmartPlayer : public Player
{
    Module_Class_Members( SmartPlayer, Player, 16384)
    virtual char *playerName();
    virtual int calculateMove( int num_sticks );
};

class InteractivePlayer : public Player
{
    Module_Class_Members( InteractivePlayer, Player, 16384)
    virtual char *playerName();
    virtual int calculateMove( int num_sticks );
};
