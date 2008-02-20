
network Error1 {
    @display("=foo");  // missing tagname
}

network Error2 {
    @display(",foo");  // missing tagname
}

network Error3 {
    @display("=;=;=");  // missing tagname (?)
}

network Error4 {
    @display("i**=foo"); // illegal tag name
}

network Error5 {
    @display("i,,=foo"); // illegal tag name (?)
}

network Error6 {
    @display("x=1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1"); // too many args
}
