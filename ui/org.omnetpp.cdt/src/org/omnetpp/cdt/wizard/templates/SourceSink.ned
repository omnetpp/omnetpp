${bannercomment}

#if($simulationspackage!="")package ${simulationspackage};#end
#if($rootpackage!="")
import ${rootpackage}.Source;
import ${rootpackage}.Sink;
#end

//
// Sample network, consisting of a source and a sink.
//
network SourceSink
{
    submodules:
        source: Source;
        sink: Sink;
    connections:
        source.out --> sink.in;
}


