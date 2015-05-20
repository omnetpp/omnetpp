%module test;

%ignore FILE;

%ignore *::print(FILE *);
%ignore *::print(FILE *, long, bool);

class X {
public:
  void print();
  void print(FILE *);
  void print(FILE *, long, bool);
};

namespace ns {
class Y {
public:
  void print();
  void print(FILE *);
  void print(FILE *, long, bool);
};
}

