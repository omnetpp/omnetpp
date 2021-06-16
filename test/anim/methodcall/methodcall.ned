
simple Mod
{
}

simple Target
{
}

module BoxedTarget
{
    submodules:
        target: Target {
            parameters:
                @display("p=70,57;i=block/sink");
        };
}

module BoxedMod
{
    submodules:
        mod: Mod {
            parameters:
                @display("p=68,69;i=block/source");
        };
        boxedTarget: BoxedTarget {
            parameters:
                @display("p=220,97");
        };
        target: Target {
            parameters:
                @display("p=220,37;i=block/sink");
        };
}

//
// Model for testing animation of method calls
//
network MethodAnimTest
{
    submodules:
        boxedMod: BoxedMod {
            parameters:
                @display("p=71,109");
        };
        boxedTarget: BoxedTarget {
            parameters:
                @display("p=230,143");
        };
        target: Target {
            parameters:
                @display("p=230,84;i=block/sink");
        };
}


