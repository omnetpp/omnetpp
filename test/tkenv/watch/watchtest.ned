
simple WatchTest
    parameters:
        num: numeric,
        boolean: bool,
        str: string;
endsimple

network watchTest : WatchTest
    parameters:
        num = 100,
        boolean = true,
        str = "foo";
endnetwork


