//------------------------------------------------------------
// File: dyna.ned
//
//  OMNeT++ Example simulation with dynamically created modules
// 
//  A.Varga, Dec 14 1996
//------------------------------------------------------------

//------------------------------------------------------------
// 
//  Basic module structure:
//      several Clients  <===>  Switch  <--->  Server
// 
//  Server launches ServerProcess processes to do actual job
// 
//------------------------------------------------------------


// Client --
//
// A client computer which periodically connects to the
// server for data exchange.
//
simple Client 
    parameters: 
        timeout : numeric, 
        conn_ia_time : numeric, 
        query_ia_time : numeric, 
        num_query : numeric; 
    gates: 
        out: out; 
        in: in; 
endsimple 


// Switch --
//
// A very simple module which models the network between
// the server and the clients.
//
simple Switch 
    parameters: 
        pk_rate : numeric; 
    gates: 
        out: out[]; 
        in: in[]; 
endsimple 


// Server --
//
// Models a simple server which accepts connections from the
// client computers. It serves multiple connections at a time; 
// each connection is handled by a ServerProcess module, created
// on demand.
//
simple Server 
    parameters: 
        processing_time : numeric; 
    gates: 
        out: out; 
        in: in; 
endsimple 


// ServerProcess --
//
// Handles one connection in the server.
//
simple ServerProcess 
    gates: 
        out: out; 
        in: in; 
endsimple 


// ClientServer --
//
// Model of the network, consisting of serveral clients, a server
// and a switch.
//
module ClientServer 
    parameters: 
        num_clients : numeric const; 
    submodules: 
        server: Server; 
            parameters: 
                processing_time = input(0.2s,"Query processing time:"); 
        switch: Switch; 
            parameters: 
                // switch_pk_rate should be >= num_clients, otherwise switch will 
                // become the bottleneck
                pk_rate = 1.5*num_clients;
            gatesizes: 
                in[num_clients+1], 
                out[num_clients+1]; 
        client: Client[num_clients]; 
            parameters: 
                timeout = 5s, 
                conn_ia_time = input(10s,"Connection interarrival time:"), 
                query_ia_time = input(2s,"Query interarrival time:"), 
                num_query = input(5,"Number of queries per conn:"); 
    connections: 
        for i=0..num_clients-1 do 
            client[i].out --> delay 10ms --> switch.in[i]; 
            client[i].in <-- delay 10ms <-- switch.out[i]; 
        endfor; 
        server.out --> delay 10ms --> switch.in[num_clients]; 
        server.in <-- delay 10ms <-- switch.out[num_clients]; 
endmodule 


// dyna --
//
// Instantiates a ClientServer network.
//
network dyna : ClientServer 
    parameters: 
        num_clients = input(4,"Number of clients:"); 
endnetwork 


