//
// Include file for scave.i
//

namespace omnetpp { namespace scave {

%ignore IDList::IDList(unsigned int);
%ignore IDList::IDList(const IDList&);

%ignore IDList::toByteArray;
%ignore IDList::fromByteArray;
%ignore IDList::begin;
%ignore IDList::end;

%typemap(javacode) IDList %{
  @Override
  public int hashCode() {
    return size(); // results in poor hashing, but IDLists are not supposed to be used as keys
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
