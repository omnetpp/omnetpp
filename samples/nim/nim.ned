//-------------------------------------------------------------
// file: nim.ned
//        (part of NIM - an OMNeT++ demo simulation)
// 
//  NIM is an ancient game with two players and a bunch of sticks.
// 
//  The players take turns, removing 1..4 sticks from the heap
//  at each turn.
// 
//  The player who must take the *last* stick is the loser.
// 
//-------------------------------------------------------------


// Game --
//
// Declaration of simple module type Game
//
simple Game
    parameters: 
        num_sticks : numeric, // starting number of sticks
        first_move : numeric; // 1=Player1, 2=Player2
    gates: 
        in: from_player1; // input and output gates
        in: from_player2; // for connecting to Player1/Player2
        out: to_player1; 
        out: to_player2; 
endsimple 


// Player --
//
// Interface declaration for different modules implementing players
//
simple Player 
    gates: 
        in: in; // gates for connecting to Game
        out: out; 
endsimple 


// Nim --
//
// The NIM game's network: Game and two players interconnected
//
module Nim 
    parameters: 
        player1_type : string, 
        player2_type : string; 
    submodules: 
        // create the Game module, with the initial number of
        // sticks and the starting player as random parameters
        game: Game; 
            parameters: 
                num_sticks = intuniform (21, 31), 
                first_move = intuniform (1, 2); 
        // for players, define module type as parameter to
        // the parent module, Nim
        player1: player1_type like Player; 
        player2: player2_type like Player; 
    connections: 
        player1.out --> game.from_player1; 
        player1.in <-- game.to_player1; 
                                        
        player2.out --> game.from_player2; 
        player2.in <-- game.to_player2; 
endmodule 


// nim --
//
// an instance of the Nim module
//
network nim : Nim 
    parameters: 
        player1_type = input("InteractivePlayer"), 
        player2_type = input("SmartPlayer"); 
endnetwork 




