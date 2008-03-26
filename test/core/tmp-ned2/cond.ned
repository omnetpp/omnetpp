
// test operator precedence, associativity (3 << 2 << 4), recursive expression (more complex)
network Cond {
    parameters:
        int i;
        string a = i==1 ? "one" : "not one";
        string b = i==1 ? "one" : (i==2 ? "two" : (i==3 ? "three" : "what???"));
        string c = i==1 ? "one" : i==2 ? "two" : i==3 ? "three" : "what???";
}