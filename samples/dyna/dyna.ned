//------------------------------------------------------------
// file: dyna.ned
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

simple Client
  parameters: timeout, conn_ia_time, query_ia_time, num_query;
  gates: out: out; in: in;
endsimple

simple Switch
  parameters: pk_rate;
  gates: out: out[]; in: in[];
endsimple

simple Server
  parameters: processing_time;
  gates: out: out; in: in;
endsimple

simple ServerProcess
  gates: out: out; in: in;
endsimple

module ClientServer
    parameters: const num_clients;
    submodules:
        server: Server
          parameters: processing_time = input(0.2s,"Query processing time:");
        switch: Switch
          parameters: pk_rate = 80;
          gatesizes: in[num_clients+1], out[num_clients+1];
        client: Client[num_clients]
          parameters: timeout = 5s,
                      conn_ia_time = input(10s,"Connection interarrival time:"),
                      query_ia_time = input(2s,"Query interarrival time:"),
                      num_query = input(5,"Number of queries per conn:");
    connections:
        for i = 0 .. num_clients-1 do
           client[i].out --> delay 10ms --> switch.in[i];
           client[i].in <-- delay 10ms <-- switch.out[i];
        endfor
        server.out --> delay 10ms --> switch.in[num_clients];
        server.in <-- delay 10ms <-- switch.out[num_clients];
endmodule

network dyna: ClientServer
    parameters: num_clients = input(4,"Number of clients:");
endnetwork
