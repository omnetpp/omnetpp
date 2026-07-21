:author: Andras
:date: 2020-09-11

Porting Real-World Protocol Implementations into OMNeT++
========================================================

Network protocols and stacks are becoming increasingly more complex nowadays, to
the point that writing a simulation model for them is often not a really viable
option due to resource constraints. An alternative to writing a model from
scratch is to reuse an existing open source real-world implementation, if one
exists. Reuse has clear advantages: beyond taking much less effort to implement,
one can also a spare significant amount of testing and validation effort. The
drawback of course is less integration with the simulation framework
(statistics, logging, etc.), difficulties with maintenance (keeping up to date
with the upstream codebase), and having to live with a Frankenstein piece of
software.

An early example of reusing real-life network code is Sam Jensen's *Network
Simulation Cradle* (NSC), which wraps various real-life TCP implementations
(the Linux, FreeBSD, OpenBSD, and lwIP ones) in order to make them accessible in
simulators such as ns-3 and OMNeT++.

In this blog post, we explore some of the workable approaches of bringing
an existing protocol implementation into an OMNeT++ simulation.

Setting out
-----------

Let's assume we have the external library that we want to use in a simulation.
Picture lwIP for example, a TCP/IP stack which includes many other protocols. To
make it usable in an OMNeT++ simulation, we'd wrap the lwIP code into a simple
module which would serve as "glue" code.

There are a few issues that obviously need to be solved, in the glue code or by
modifying the lwIP code, to make it work: input/output, timers, configuring,
statistics, etc. That is, the packets lwIP sends and receives need to be
converted to/from OMNeT++ messages; socket API calls made in the simulation need
to find their way to the appropriate functions of lwIP; timers in the lwIP code
need to map to OMNeT++ self messages; configuration possibilities of lwIP need
to be mapped to OMNeT++ module parameters; statistics collected inside lwIP (if
there are any -- possibly they need to be added) need to be exposed as OMNeT++
signals. This is all fairly straightforward, but not always easy. It is
hard to formulate generic advice on those topics, because the solution tends to
depend very much on the internals of the library to be wrapped.

Instead, in the rest of this post we deal with a less obvious problem that looks
easy on the surface but tends to cause the most headache in practice: the issue
of global variables.

Most real-life software assumes it runs as a sole instance in its process, and
therefore uses global variables to access its state such as configuration,
connection tables, etc. However, in the simulation we typically want to create
multiple instances: we want to simulate multiple routers, switches, hosts, etc.
in a network. Each instance should have (sticking to the lwIP example) its own
routing table, list of open sockets, etc. Thus, we need separate sets of global
variables for each lwIP instance, and make sure each instance uses its own set.

There are a few distinct approaches to solve this problem, such as:

1. Packaging the software as a shared library, and loading it at runtime as many
   times as the number of instances you need. The dynamic loader of your system
   would take care that each loaded instance is a self-contained one, having no
   data common with the other instances.

2. Exploiting object-orientation. If the library is implemented in C++, odds are
   that creating multiple instances is only as much as instantiating the
   corresponding classes multiple times. Or, if the library is programmed in C,
   it could be feasible to wrap the code into C++ classes.

3. Introducing indirection, i.e. collecting all global variables into a struct
   and modifying the library to access them via a pointer, a single global
   variable that we always set to point to the appropriate set of variables
   before calling into the library.

4. Copying, that is, we maintain a "backup" (or "shadow", if you like) set of
   global variables of each instance, and we copy its contents in/out of the
   actual global variables before and after each call into the library.

In the following sections we explore these options.

Loading the library multiple times
----------------------------------

If the library is (or can be) compiled to a shared library, the first idea is to
load it multiple times. Then, for example, if you have 10 routers in the
simulation and each one needs one instance of the protocol the library
implements, you would load the library 10 times, in such a way that the dynamic
linker of your system ensures that each loaded instance is a self-contained one,
with no data common with the other instances.

Sounds easy. The first hurdle is that simple ``dlopen()`` won't work: when the
library is already loaded, further ``dlopen()`` calls just return the handle of
the already loaded instance. (This is also true if the first instance of the
library was loaded as part of the program and not via ``dlopen()``.)

It is possible to fool ``dlopen()`` by making several copies of the shared library
on the disk under different names, but this solution is neither elegant nor very
efficient, even if you try soft or hard links to spare disk space instead of
copying. (Still, I've seen projects follow this approach; see
https://github.com/simgrid/simgrid/issues/137.)

A better alternative is to use ``dlmopen()``. The ``dlmopen()`` function differs from
``dlopen()`` primarily in that it accepts an additional argument that
specifies the link-map list (also referred to as a namespace) in which the
shared object should be loaded. When ``LM_ID_NEWLM`` is specified in this argument,
the library will be loaded into a new empty namespace, allowing multiple
instances of the library to be loaded.

.. code-block:: c

    void *handle = dlmopen(LM_ID_NEWLM, "libfoo.so", RTLD_NOW);

An inconvenience of using libraries via ``dlopen()``/``dlmopen()`` is that symbols
(such as function names) in the library are not directly available in the parent
program, but ``dlsym()`` needs to be used to resolve them. A more serious limitation
is that on the system we tested, ``dlmopen()`` only allowed 15 instances of
the library to be loaded. This limit can be raised (up to 256?), but loading
the library multiple times certainly doesn't scale to hundreds or thousands
of instances.

Exploiting object orientation
-----------------------------

Porting C++ libraries
~~~~~~~~~~~~~~~~~~~~~

If the library was written in C++ (as opposed to pure C), chances are that
functionality is nicely encapsulated into a collection of classes. If the
library is well-designed, one can just create as many instances of the network
stack (or the component is question) as needed.

Except of course, if the library makes use of the dreaded Singleton pattern, or
otherwise uses global variables. However, singletons are usually easy to
identify in the code, and the source can be modified to allow several instances
to coexist in memory.

In the rest of this post, we assume that the library was written in C.

Wrapping into C++
~~~~~~~~~~~~~~~~~

That fact that library was written in C doesn't necessarily mean in cannot be
turned into (non-idiomatic) C++ with some effort. After all, most C code
compiles as C++ just fine. If you manage to wrap the code into a C++ class (so
that C global variables become data members of the C++ class, and C functions
become member functions of the same class), it will be easy to instantiate it in
multiple instances. Notice that function bodies won't need to be modified much:
since ``this->`` is implicit for member access, C variable accesses and function
calls will transparently compile as member accesses and calls in C++.

What needs to be changed? Assuming that all important definitions in the library
are in a single header file, adding ``class Foo {`` (replace ``Foo`` with a name of
your choice) near the top and ``};`` near the bottom of the file will turn types
into nested types, and variable declarations and functions into class members.
(Variable declarations need a little massaging: ``extern`` needs to be removed,
and the initial value brought over from the implementation (``.c``) file.) The
corresponding ``.c`` file should be renamed to ``.cc`` so that it is compiled as
C++, functions definitions (and possibly some more elements) in it need to be
decorated with ``Foo::``, and global variable definitions commented out (ensuring
they are declared as class members now). Some constructs such as static
functions and static variables inside functions (= global variables only visible
inside the function) need extra care. The resulting code should largely compile
without any further changes, notably without having to change function bodies.
In practice, subtle incompatibilities between C and C++ often cause compilation
errors, but they are usually not hard to fix.

An example of this approach is how lwIP was ported into INET Framework.

Introducing indirection
-----------------------

An extra level of indirection solves everything, right? Maybe not, but here it
really helps. In this solution, we create a separate set of global variables for
each instance, and modify the library to access the variables via a pointer
(which we define as a global variable so it doesn't need to be passed around).
Before each call into the library, we set the pointer to the set of globals of
the correct instance.

There are several ways to achieve this effect. Let's see them!

Indirection by modifying the source
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The most obvious solution is to modify the source code: collect all global
variables into a struct type as fields, and prefix all accesses of the
individual global variables with ``ptr->`` (better names for the pointer can
certainly be invented). Now the library will access its "global" variables via
the ``ptr`` pointer (it needs to be defined as a global variable of course).
Now, one can create and use an arbitrary number of instances from the library by
allocating a globals struct for each, and setting the pointer to point to right
one before each call.

Note that private global variables, such as those declared ``static`` in C files
and inside function bodies, so they'll also need a place in the struct.

The difficulty with this solution is of course the need to modify each and every
global variable access in the code. The task can be pulled off by hand
(tedious), or with tools like Sam Jensen's _globalizer_ tool. The number of
textual changes could be reduced by the use of macros (``#define foo
ptr->m_foo``), but that often causes more problems than it solves (the name of a
global variable may also occur as the name of a parameter, local variable or
even a type, causing weird compile errors).

Another drawback is that the above code changes are usually quite intrusive,
in the sense that they are not local, but occur all over the codebase. Chances
are that when you want to upgrade to the next version of the library,
the original "globalization" patch won't apply, and you'll end up having
to do the replacements all over again.

Indirection by code instrumentation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A definitely more hardcore solution is to intervene during the compilation of
the library, and add the indirection then. It may sound like science fiction,
but quite surprisingly, it is actually feasible.

They key is to add a plugin to the Clang compiler to perform a new compilation pass.
The pass will operate on the LLVM intermediate representation (IR) of the compilation
unit. It can iterate over all global variable accesses in the code, and modify each
one to introduce the indirection by a pointer global variable. The struct of global
variables can also be created during the compiler pass. This may sounds straightforward,
but there are a number of subtle problems to solve along the way. For example,
since the C++ compiler processes each file in isolation, it is not easy to decide
in which file to place the new global variables so as not to cause multiple definitions.

Attila Török in our team has experimented with this approach, and while not solving
it completely, he has gotten quite far with it. If you are interested in picking
up where he left off, drop us an email.

The drawback of this (otherwise very elegant) solution is the deployment. If you
want your users to be able to compile the library, they'll need the compiler plugin.
And if you distribute the compiler plugin in source form (it is written in C++),
they'll need to install the ``clang-devel`` package to be able to build it.

Indirection by hacking the dynamic linker
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There might be a third solution as well, involving the dynamic linker. When
accessing functions and global variables loaded from a shared library, there is
already an indirection in place: all such accesses go through the *Global Offset
Table* (GOT). In theory, it could be possible to update the corresponding
entries in the GOT to point to the desired set of global variables before each
call into the library. For that matter, it could also be possible to have
separate instances of the GOT table for each library instance, and just
activate the right GOT before calls into the library.

So far we haven't figured out enough about the GOT data structure and the
dynamic loader in general to judge whether the above idea is feasible or not. It
is unclear whether there is enough information available at runtime to identify
the relevant entries in the GOT, or even, whether GOT is writable at all. One
thing is certain: such as solution, even if it worked, would be extremely
platform dependent.

Copying
-------

The alternative to indirection is copying. In this approach, we maintain a
"shadow" set of global variables for each library instance. Before before each call
into the library, we populate the global variables from the instance's "shadow" set,
and after the call, we update the "shadow" set by copying the global variables
into it.

Whenever a new library instance is needed, we populate the new "shadow" set from
a pristine backup copy that we saved early, before the first call into the
library. This is possible to do with C libraries because, unlike C++ libraries,
they normally don't contain initialization code that automatically runs on
loading the library. (In C++, constructors of global objects are run
automatically on library load, which makes things tricky. In C, global variables
are only allowed to have constant initializers. While it is also possible in C
to create initialization functions run automatically on library load, e.g. via
``__attribute__ ((constructor))``, that is not very common.)

The cost of copying before/after calls may or may not be significant, depending
on how the library was written. If the library mostly uses dynamically allocated
data structures, the cost is small. Static allocations are the problem. Static
buffers, for example, can add a lot to the amount of memory that needs to be
copied. In such cases, modifying the library to allocate the buffers dynamically
can make a lot of difference in performance.

While the copying approach is certainly less efficient than the indirection
approach (where all it takes to switch instance is to change the value of a
single pointer), it is significantly easier to implement, and requires fewer
(and less intrusive) modification to the library source code.

There are several ways this approach can be implemented.

Copying variables one by one
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A straightforward approach is to collect the global variables of the library
into a struct type that represents the "shadow" set (while leaving the original
source intact), and write two functions that contain a series of assignments:
one would copy the global variables into a struct, and the other would copy
in the reverse direction.

If library contains global variables that are not accessible to the copying
functions (because they are marked ``static`` and are local to a file or are
inside functions), such places need to be modified in the library source.

The drawback of this solution is that it requires you to collect the list of
global variables (which requires attention when you upgrade to a newer version
of the library), and may also require source code changes to the library. The
next approach is also not without problems, but it eliminates both these issues.

Copying the library's data segment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is possible to automate the above approach. Global variables of a shared
library form a contiguous region in the memory, so it we know the range of
addressses it occupies, we can copy all of them in one go, using a simple
``memcpy()`` call.

The difficult problem is how to determine the address range. Unfortunately there
is no "official" solution.

Our first idea was this: if the compiler preserves the order of variables within
a compilation unit, we can place guard variables like ``startVars`` and ``endVars`` at
the top and bottom of each source file, and the address range between them will
include all global variables. Unfortunately, the assumption was wrong: variables
seemed to appear in the memory in an arbitrary order. Later we learned gcc has a
``-fno-toplevel-reorder`` option for exactly this purpose.

Similarly, if the linker preserves the order the object files appear on its
command line, it should be possible to surround the list objects files with a
``startvars.o`` and an ``endvars.o`` file that contain similarly named guard
variables. Unfortunately, it also turned out that the actual order of variables
in the shared library does not necessarily reflect their original order on the
command line. And, the linker does not appear to have an analogous
``do-not-reorder`` option.

The next option we explored was to get the address range from the dynamic
linker. Indeed, there is a ``dl_iterate_phdr()`` API function that enumerates
the loaded shared libraries and the segments in each. We figured out that global
variables are in the segment that has type 1 (PT_LOAD) and flags 6 (R+W).
However, getting the address range of that segment was not much use to us.
First, it points to readonly memory within the area where the shared object file
was loaded by the dynamic linker, and not to the region where the library's
actual variables are. Second, it includes important internal data structures (a
few hundred bytes at the start) in addition to the global variables, and
apparently those data structures should not be messed with. These hurdles,
especially the second one, could not be overcome.

The third idea that finally worked was to use a linker script to insert the
guard variables. The main purpose of the linker script is to describe for the
linker how the sections in the input files (object files, libraries, etc) should
be mapped into the output file, and to control the memory layout of the output
file. Linker scripts are normally considered an internal matter of the linking
process, but it's possible to export one, modify it, and let the linker use the
modified version. For example, by editing the ``.data``  block, and adding
``startmarker.o(.data);`` at the top and ``endmarker.o(.data);`` at the bottom, one
can achieve that the data of the two named object files (``startmarker.o`` and
``endmarker.o``) are placed at the top and bottom of the data segment. If those
files contain guard variables, their address range at runtime denotes the data
area of the library. Since we stopped experimenting before reaching a conclusion,
it is still unclear whether this approach can be made to work.

Conclusion
----------

With so many options, the question is which approach you should choose in your
project. Here are a few guidelines.

- Multiple loading is relatively straightforward, and the required effort does
  not depend of on the size of the codebase (huge libraries are just as easy to
  load as small ones), but is limited in the number of instances you can create
  (usually a few dozen). Therefore, it is practical for libraries which have a
  large codebase or whose source is not available -- provided that it doesn't
  need to be instantiated on a massive scale.
- Copying is not very efficient (usually there too many variables, static
  tables, etc). Simulations using that approach may be slow.
- Wrapping into C++ might not be practical with C code that contains a lot of
  constructs that don't compile as C++ (too much patching required).

Choose your own weapon.
