// unresolved base class etc

simple Bogus1 extends Yerewr { }
module Bogus2 extends Qwerty { }
channel Bogus3 extends Rreterty { }
network Bogus4 extends Tewrrwr { }

simple BogusLike1 like Yerewr { }
module BogusLike2 like Qwerty { }
channel BogusLike3 like Rreterty { }
network BogusLike4 like Tewrrwr { }

moduleinterface IBogus1 extends Bgfhgf { }
channelinterface IBogus2 extends Otytr { }

network Net {
    types:
        simple Bogus1 extends Yerewr { }
        module Bogus2 extends Qwerty { }
        channel Bogus3 extends Rreterty { }
        network Bogus4 extends Tewrrwr { }

        simple BogusLike1 like Yerewr { }
        module BogusLike2 like Qwerty { }
        channel BogusLike3 like Rreterty { }
        network BogusLike4 like Tewrrwr { }

        moduleinterface IBogus1 extends Bgfhgf { }
        channelinterface IBogus2 extends Otytr { }
}
