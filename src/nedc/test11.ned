//
// Test 11: submodule type as parameter
//

simple Player
        gates:
          in: in;
          out: out;
endsimple

module Game
    parameters:
        player1_type: string,
        player2_type: string;
    submodules:
        player1: player1_type like Player;
        player2: player2_type like Player;
    connections:
        player1.out --> player2.in;
        player1.in  <-- player2.out;
endmodule

network game: Game
    parameters:
        player1_type = "InteractivePlayer",
        player2_type = "SmartPlayer";
endnetwork
