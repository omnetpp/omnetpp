while (<>) {
    s|``|<literal>|g;
    s|''|</literal>|g;
    print;
}