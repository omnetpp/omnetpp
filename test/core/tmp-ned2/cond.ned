
// test operator precedence, associativity (3 << 2 << 4), recursive expression (more complex)
network Cond {
    parameters:
        int i;
        string a = i==1 ? "one" : "not one";
        string b = i==1 ? "one" : (i==2 ? "two" : (i==3 ? "three" : "what???"));
        string c = i==1 ? "one" : i==2 ? "two" : i==3 ? "three" : "what???";
}

both in parameter value and connections part
index - index of module in parent's vector
sizeof - gate/submodule vector, own or another submodule's
this - submodule's parameter (by default parameter names refer to parent's p..)
variable reference either refers to a cycle variable or a compound module parameter
connections may refer to submodule parameters (submodname.paramname)

like might be an expression and may come from ini file
<> may be specified by *.typename = in the ini file

string concatenation using +
