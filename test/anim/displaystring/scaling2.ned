// verify that positions and sizes are not accidentally rounded or truncated to integers
module scaling2
{
    parameters:
        @display("bgb=5,3;bgg=1,2");
    types:
        module Host {
            double x = default(0);
            double y = default(0);
            double height = default(0);
            double width = default(0);
            double radius = default(0);
        }
    submodules:
        label: Dummy {
            @display("b=0,0;t=Note: Hosts should form a straight line\nfrom the top-left corner to the bottom-right one");
        }
        host[100]: Host {
            @display("i=device/laptop_s;p=$x,$y");
            x = index*0.05;
            y = index*0.03;
        }

        rect[10]: Host {
            @display("b=$width,$height;r=$radius;p=1,1,m,1,0,0.2");
            width = index*0.1;
            height = index*0.05;
            radius = index*0.15;
        }
}
