network SixNodes extends Base
{
    submodules:
        tic[6]: Node {
            parameters:
                @display("i=ball2");
        };
    connections:
        tic[0].out++ --> tic[1].in++;
        tic[0].in++ <-- tic[1].out++;

        tic[0].out++ --> tic[3].in++;
        tic[0].in++ <-- tic[3].out++;

        tic[1].out++ --> tic[2].in++;
        tic[1].in++ <-- tic[2].out++;

        tic[1].out++ --> tic[3].in++;
        tic[1].in++ <-- tic[3].out++;

        tic[1].out++ --> tic[4].in++;
        tic[1].in++ <-- tic[4].out++;

        tic[3].out++ --> tic[4].in++;
        tic[3].in++ <-- tic[4].out++;

        tic[4].out++ --> tic[5].in++;
        tic[4].in++ <-- tic[5].out++;
}

