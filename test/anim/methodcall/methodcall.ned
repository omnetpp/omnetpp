
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
                @display("b=32,30;p=70,57;i=sink");
        };
}

module BoxedMod
{
    submodules:
        mod: Mod {
            parameters:
                @display("b=32,30;p=68,69;i=gen");
        };
        boxedTarget: BoxedTarget {
            parameters:
                @display("p=220,97;b=40,28");
        };
        target: Target {
            parameters:
                @display("b=32,30;p=220,37;i=sink");
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
                @display("p=71,109;b=40,28");
        };
        boxedTarget: BoxedTarget {
            parameters:
                @display("p=230,143;b=40,28");
        };
        target: Target {
            parameters:
                @display("b=32,30;p=230,84;i=sink");
        };
}


