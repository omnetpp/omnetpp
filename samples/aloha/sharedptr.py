import cppyy

cppyy.cppdef(""" 
   #include <iostream> 
   #include <memory> 
   class Foo { 
   public: 
      Foo() {std::cout << "created Foo at " << (void*)this << std::endl;} 
      ~Foo() {std::cout << "deleted Foo at " << (void*)this << std::endl;}  
      void hello() {std::cout << "hello from Foo at " << (void*)this << std::endl;}  
   }; 
   std::shared_ptr<Foo> makeFoo() {return std::make_shared<Foo>();} 
   std::shared_ptr<Foo> theFoo;
   """)


from cppyy.gbl import makeFoo, theFoo

print("case 1")
a = makeFoo()
del a

print("a = makeFoo()")
a = makeFoo()
print("theFoo = a")
theFoo = a
print("del a")
del a
print("del theFoo")
del theFoo

