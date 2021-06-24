
module WatchTest
{
    parameters:
        @class(WatchTest);
        @isNetwork(true);
        double num = 100;
        bool boolean = true;
        string str = "foo";
    submodules:
        sub : Sub;
}

simple Sub
{

}