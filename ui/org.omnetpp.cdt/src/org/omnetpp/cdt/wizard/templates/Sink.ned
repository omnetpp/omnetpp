${bannercomment}

#if($rootpackage!="")package ${rootpackage};#end

//
// Consumes received messages and collects statistics
//
simple Sink
{
    parameters:
        @display("i=block/sink");
    gates:
        input in;
}

