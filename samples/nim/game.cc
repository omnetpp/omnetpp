//-------------------------------------------------------------
// file: game.cc
//        (part of NIM - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "omnetpp.h"

// derive Game from cSimpleModule
class Game : public cSimpleModule
{
    Module_Class_Members(Game,cSimpleModule,16384)
       // this is a macro; it expands to constructor definition etc.
       // 16384 is the size for the coroutine stack (in bytes)

    virtual void activity();
       // this redefined virtual function holds the algorithm
};

// register the simple module class to OMNeT++
Define_Module( Game );

// Operation of Game:
//  The Game module first waits for a message from both players and extracts
//  the message names that are also the players' names. Then it enters a loop,
//  with the player_to_move variable alternating between 1 and 2. With each
//  iteration, it sends out a message with the current number of sticks to
//  the corresponding player and gets back the number of sticks taken by that
//  player. When the sticks are out, the module announces the winner and ends
//  the simulation.

void Game::activity()
{
    // strings to store player names; [0] is unused
    char player[3][32];

    // read parameter values
    int num_sticks = par("num_sticks");
    int player_to_move = par("first_move");

    // make the variables visible from Tkenv
    WATCH(num_sticks);
    WATCH(player_to_move);

    // waiting for players to tell their names
    ev << "Waiting for players to tell their names...\n";
    for (int i=0; i<2; i++)
    {
        cMessage *msg = receive();
        if (msg->arrivedOn("from_player1"))
                strcpy( player[1], msg->name());
        else
                strcpy( player[2], msg->name());
        delete msg;
    }

    ev << "OK, let the game begin!\n";
    ev << "Player 1: " << player[1] << "   Player 2: " << player[2]
       << "\n\n";

    // main loop of the game
    do
    {
        ev << "Sticks left: " << num_sticks << "\n";
        ev << "Player " << player_to_move << " ("
           << player[player_to_move] << ") to move.\n";

        // send out how many sticks we have
        char msgname[32];
        sprintf(msgname,"%d sticks left",num_sticks);
        cMessage *msg = new cMessage(msgname);
        msg->setKind(num_sticks);

        if (player_to_move == 1)
                send(msg, "to_player1");
        else
                send(msg, "to_player2");

        // wait for the player to answer how many he takes
        msg = receive();
        int sticks_taken = msg->kind();
        delete msg;

        num_sticks -= sticks_taken;

        ev << "Player " << player_to_move << " ("
           << player[player_to_move] << ") took "
           << sticks_taken << " stick(s).\n";

        // switch to the other player
        player_to_move = 3 - player_to_move;
    }
    while (num_sticks>0);

    ev << "\nPlayer " << player_to_move << " ("
       << player[player_to_move] << ") won!\n";

    endSimulation();
}

