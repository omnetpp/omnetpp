//--------------------------------------------
// file: pvmex.ned
//
//  a distributed sender-receiver example
//--------------------------------------------

simple smSender
        parameters:  no_msg, dest;
        gates:       out: outgate;
endsimple

simple smReceiver
        gates:     in: ingate;
endsimple

module SenderReceiver
        machines: sndrhost, recvhost;
        submodules:
                sender: smSender
                    on:         senderhost;
                    parameters: no_msg=10, dest=1;
                receiver: smReceiver
                    on:         recvhost;
       connections:
                sender.outgate --> receiver.ingate;
endmodule

network SenderReceiverNet: SenderReceiver
             on:  sndrhost, recvhost;
endnetwork

