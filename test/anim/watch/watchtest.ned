
simple WatchTest
{
    parameters:
        double num;
        bool boolean;
        string str;
}

network watchTest extends WatchTest
{
    parameters:
        num = 100;
        boolean = true;
        str = "foo";
}


