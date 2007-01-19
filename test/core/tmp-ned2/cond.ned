network Cond {
    parameters:
        int i;
        string x = i==1 ? "one" : i==2 ? "two" : i==3 ? "three" : "what???";
}