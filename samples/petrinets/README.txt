This models a stochastic Petri net.

TODO: this should probably be released separately, and not as a sample

TODO: make separate immediate and timed transitions? different icon etc!

TODO: scheduling is currently not fair!! if some transition draws prio=999, it will starve!!

TODO: make Eclipse plugin that converts PNML to NED (and back?)

TODO: how to connect it to other modules, e.g. the Queueing lib? E.g.: special Transition that sends a message when fires

TODO: collect statistics!!!

TODO: problem with timed transitions: when do we generate tokens on the output???
        race condition! (if early: then other transitions can fire immediately;
        if late? there may not be enough room in the output places! reserve?)

TODO: ResourceTransition -- resource-ot kell allokalnia hogy el tudjon sulni