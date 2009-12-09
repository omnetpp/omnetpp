module Subnet {
    @display("bgg=20;i=misc/cloud");
    submodules:
        controller : Controller2;
}

network Net2
{
    @display("bgg=50");
    submodules:
        controller : Controller2;
        subnet1 : Subnet;
        //subnet2 : Subnet;
}
