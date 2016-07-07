module Subnet {
    @display("bgg=20;i=misc/cloud;bgb=600,400");
    submodules:
        controller : Controller2;
}

network Net2
{
    @display("bgg=50;bgb=600,400");
    submodules:
        controller : Controller2;
}
