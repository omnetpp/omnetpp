
simple Mod
{
}

module Box1
{
    submodules:
        mod: Mod {
            parameters:
                @display("b=32,30;p=68,69;i=gen");
        };
}

module Box2
{
}

network ChangeParent
{
    submodules:
        box1: Box1 {
            parameters:
                @display("p=71,109;b=40,28");
        };
        box2: Box2 {
            parameters:
                @display("p=230,143;b=40,28");
        };
}


