//
// Include file for scave.i
//

namespace omnetpp { namespace scave {

%ignore IDList::IDList(unsigned int);
%ignore IDList::IDList(const IDList&);

%ignore IDList::begin;
%ignore IDList::end;

%feature("novaluewrapper") IDList;

// when returning an IDList by value, ensure that in the generated C++ code (scave.cxx), the result is moved instead of copied (performance) 
%typemap(out) IDList {
    *(omnetpp::scave::IDList **)&jresult = new omnetpp::scave::IDList(std::move(result)); // custom code -- move instead of copy 
  }


%typemap(javacode) IDList %{
  @Override
  public int hashCode() {
    long h = hashCode64();
    return (int)(h ^ (h>>>32));
  }

  @Override
  public String toString() {
      String result = "IDList of size " + size() + ", first elements: ";
      for (int i = 0; i < 10; ++i)
          if (i < size())
              result += get(i) + ", ";
        return result;
  }
%}


} } // namespaces

%include "scave/idlist.h"
