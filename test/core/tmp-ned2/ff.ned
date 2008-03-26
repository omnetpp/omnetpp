
// tests inner type parameter settings
network FF
{
    types:
        module Node
        {
            parameters:
                string p = default("default");
        }
    submodules:
        b: Node {};
        a: Node {};

        a1: Node {};
        b1: Node {};
}


