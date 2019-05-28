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


%typemap(javaimports) IDList %{
import java.util.ArrayList;
import java.util.EventListener;
%}


%typemap(javacode) IDList %{

  public static interface IChangeListener extends EventListener {
    public void idlistChanged(IDList idlist);
  }

  private ArrayList<IChangeListener> listeners = new ArrayList<IChangeListener>();

  // TODO ensure that immutable
  public static final IDList EMPTY = new IDList();

  public void addListener(IChangeListener listener) {
      listeners.add(listener);
  }

  public void removeListener(IChangeListener listener) {
      listeners.remove(listener);
  }

  protected void notifyListeners() {
      for (IChangeListener listener : listeners.toArray(new IChangeListener[listeners.size()]))
          listener.idlistChanged(this);
  }

  public void swigDisown() {
      swigCMemOwn = false;
  }

  public static IDList fromArray(Long[] array) {
      IDList list = new IDList();
      for (int i=0; i<array.length; i++)
          list.add(array[i].longValue());
      return list;
  }

  public Long[] toArray() {
      int sz = (int) size();
      Long[] array = new Long[sz];
      for (int i=0; i<sz; i++)
          array[i] = Long.valueOf(get(i));
      return array;
  }

  @Override
  public int hashCode() {
    return size(); // results in poor hashing, but IDLists are not supposed to be used as keys
  }

%}

%define ADD_NOTIFY(METHOD)
%typemap(javaout) void omnetpp::scave::IDList::METHOD {
    $jnicall;
    notifyListeners();
  }
%enddef

ADD_NOTIFY(add);
ADD_NOTIFY(intersect);
ADD_NOTIFY(merge);
ADD_NOTIFY(subtract);
ADD_NOTIFY(sortByDirectory);
ADD_NOTIFY(sortByFileName);
ADD_NOTIFY(sortByModule);
ADD_NOTIFY(sortByName);
ADD_NOTIFY(sortByRun);
ADD_NOTIFY(sortScalarsByValue);
ADD_NOTIFY(sortVectorsByLength);
ADD_NOTIFY(sortVectorsByMean);
ADD_NOTIFY(sortVectorsByStdDev);
ADD_NOTIFY(sortVectorsByVectorId);
ADD_NOTIFY(sortVectorsByMin);
ADD_NOTIFY(sortVectorsByMax);
ADD_NOTIFY(sortVectorsByStartTime);
ADD_NOTIFY(sortVectorsByEndTime);
ADD_NOTIFY(sortByRunAttribute);

} } // namespaces

%include "scave/idlist.h"
