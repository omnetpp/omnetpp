// test inner types, and that they see each other
network Inner {
    types:
        module Core {
        }
        module Node {
            submodules:
                c: Core {}
        }
    submodules:
        node: Node {}
}


