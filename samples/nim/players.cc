//-------------------------------------------------------------
// file: players.cc
//        (part of NIM - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "omnetpp.h"
#include "players.h"

// register player module types with the Player interface
Define_Module_Like( SmartPlayer, Player )
Define_Module_Like( SimplePlayer, Player )
Define_Module_Like( InteractivePlayer, Player )


// Operation of Player:
//  A Player first introduces himself by sendings its namestr to the Game
//  module. Then it enters an infinite loop; with each iteration, it receives
//  a message from Game with the number of sticks left, calculates its move
//  and sends back a message containing the move.

void Player::activity ()
{
    // initialisation phase: send module type to Game module
    cMessage *msg = new cMessage( playerName() );  // create a message
    send (msg, "out");                             // send it to Game

    // infinite loop to process moves; simulation will be terminated by Game
    for (;;)
    {
        // messages have several fields; here, we'll use the message kind
        // member to store the number of sticks
        cMessage *msgin = receive();        // receive message from Game
        int num_sticks = msgin->kind();    // extract message kind (an int)
                                           // this hold the number of sticks
                                           // still on the stack
        delete msgin;                      // dispose of the message

        // call virtual function (implemented in cSmartPlayer etc.)
        // to calculate the number of sticks to take.
        int move = calculateMove( num_sticks );

        ev << playerName() << " is taking " << move <<
              " out of "  << num_sticks << " sticks.\n";

        // send answer back to the Game module
        char msgname[32];
        sprintf(msgname,"Taking %d", move);
        cMessage *msgout = new cMessage(msgname); // create message
        msgout->setKind( move );

        send( msgout, "out");       // send the message to Game
    }
}

char *SimplePlayer::playerName()
{
    return "SimplePlayer";
}

int SimplePlayer::calculateMove( int num_sticks )
{
    // random choice, at least 1, at most 4
    return 1 + intrand( num_sticks<4 ? num_sticks : 4 );
}

char *SmartPlayer::playerName()
{
    return "SmartPlayer";
}

int SmartPlayer::calculateMove( int num_sticks )
{
    // calculate move
    int move = (num_sticks + 4) % 5;

    // if it comes out to be zero, it needs to be adjusted to one;
    // this means that the other player is in the winning position
    if (move == 0)  move = 1;
    return move;
}

char *InteractivePlayer::playerName()
{
    return "InteractivePlayer";
}

int InteractivePlayer::calculateMove( int num_sticks )
{
    // ask the user interactively
    int move;
    int max_sticks = num_sticks<4 ? num_sticks : 4;

    char prompt[64];
    sprintf(prompt,"%d sticks left. How many do you take (1..%d)?", num_sticks,max_sticks);
    ev.setPrompt(prompt);
    ev >> move;
    while (move<1 || move>max_sticks)
    {
        ev.printfmsg("Please enter a number in range 1..%d", max_sticks);
        ev >> move;
    }
    return move;
}
