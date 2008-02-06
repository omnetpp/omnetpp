//
// SWIG typemaps for std::list
// Andras Varga
// 2005
//
// Java implementation


%include exception.i

// containers

// methods which can raise are caused to throw an IndexError
%exception std::list::get {
    try {
        $action
    } catch (std::out_of_range& e) {
        SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, const_cast<char*>(e.what()));
        return $null;
    }
}

%exception std::list::set {
    try {
        $action
    } catch (std::out_of_range& e) {
        SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, const_cast<char*>(e.what()));
        return $null;
    }
}


// ------------------------------------------------------------------------
// std::list
//
// The aim of all that follows would be to integrate std::list with
// Java as much as possible, namely, to allow the user to pass and
// be returned Java (arrays? containers?)
// const declarations are used to guess the intent of the function being
// exported; therefore, the following rationale is applied:
//
//   -- f(std::list<T>), f(const std::list<T>&), f(const std::list<T>*):
//      the parameter being read-only, either a Java sequence or a
//      previously wrapped std::list<T> can be passed.
//   -- f(std::list<T>&), f(std::list<T>*):
//      the parameter must be modified; therefore, only a wrapped std::list
//      can be passed.
//   -- std::list<T> f():
//      the list is returned by copy; therefore, a Java sequence of T:s
//      is returned which is most easily used in other Java functions
//   -- std::list<T>& f(), std::list<T>* f(), const std::list<T>& f(),
//      const std::list<T>* f():
//      the list is returned by reference; therefore, a wrapped std::list
//      is returned
// ------------------------------------------------------------------------

%{
#include <list>
#include <algorithm>
#include <stdexcept>
%}

// exported class

namespace std {

    template<class T> class list {
        // add generic typemaps here
        %typemap(javacode) list<T> %{
          public T[] toArray() {
            int sz = (int)size();
            T[] array = new T[sz];
            for (int i=0; i<sz; i++)
              array[i] = get(i);
            return array;
          }
        %}
      public:
        list();
        unsigned int size() const;
        %rename(isEmpty) empty;
        bool empty() const;
        void clear();
        %rename(add) push_back;
        void push_back(const T& x);
        void push_front(const T& x);
        const T& back();
        const T& front();
        %extend {
            T& pop_back() {
                if (self->empty())
                    throw std::out_of_range("pop invoked on empty list");
                T& x = self->back();
                self->pop_back();
                return x;
            }
            T& pop_front() {
                if (self->empty())
                    throw std::out_of_range("pop invoked on empty list");
                T& x = self->back();
                self->pop_back();
                return x;
            }
            T& get(int i) {
                int size = int(self->size());
                if (i<0 || i>=size)
                    throw std::out_of_range("list index out of range");
                std::list<T>::iterator it = self->begin();
                for (; i>0; i--) it++;  // linear time!
                return (*it);
            }
            void set(int i, const T& x) {
                int size = int(self->size());
                if (i<0 || i>=size)
                    throw std::out_of_range("list index out of range");
                std::list<T>::iterator it = self->begin();
                for (; i>0; i--) it++;  // linear time!
                (*it) = x;
            }
            void insert(int i, const T& x) {
                int size = int(self->size());
                if (i<0 || i>size)  // i==size allowed, for append operation
                    throw std::out_of_range("list index out of range");
                std::list<T>::iterator it = self->begin();
                for (; i>0; i--) it++;  // linear time!
                self->insert(it, x);
            }
            void del(int i) {
                int size = int(self->size());
                if (i<0 || i>=size)
                    throw std::out_of_range("list index out of range");
                std::list<T>::iterator it = self->begin();
                for (; i>0; i--) it++;  // linear time!
                self->erase(it);
            }
        }
    };


    // specializations for built-ins

    %define specialize_std_list(T)
    template<> class list<T> {
        // add generic typemaps here
        %typemap(javacode) list<T> %{
          public T[] toArray() {
            int sz = (int)size();
            T[] array = new T[sz];
            for (int i=0; i<sz; i++)
              array[i] = get(i);
            return array;
          }
        %}
      public:
        list();
        unsigned int size() const;
        %rename(isEmpty) empty;
        bool empty() const;
        void clear();
        %rename(add) push_back;
        void push_back(T x);
        void push_front(T x);
        T back();
        T front();
        %extend {
            T pop_back() {
                if (self->empty())
                    throw std::out_of_range("pop invoked on empty list");
                T x = self->back();
                self->pop_back();
                return x;
            }
            T pop_front() {
                if (self->empty())
                    throw std::out_of_range("pop invoked on empty list");
                T x = self->back();
                self->pop_back();
                return x;
            }
            T get(int i) {
                int size = int(self->size());
                if (i<0 || i>=size)
                    throw std::out_of_range("list index out of range");
                std::list<T>::iterator it = self->begin();
                for (; i>0; i--) it++;  // linear time!
                return (*it);
            }
            void set(int i, T x) {
                int size = int(self->size());
                if (i<0 || i>=size)
                    throw std::out_of_range("list index out of range");
                std::list<T>::iterator it = self->begin();
                for (; i>0; i--) it++;  // linear time!
                (*it) = x;
            }
            void insert(int i, T x) {
                int size = int(self->size());
                if (i<0 || i>size)  // i==size allowed, for append operation
                    throw std::out_of_range("list index out of range");
                std::list<T>::iterator it = self->begin();
                for (; i>0; i--) it++;  // linear time!
                self->insert(it, x);
            }
            void del(int i) {
                int size = int(self->size());
                if (i<0 || i>=size)
                    throw std::out_of_range("list index out of range");
                std::list<T>::iterator it = self->begin();
                for (; i>0; i--) it++;  // linear time!
                self->erase(it);
            }
        }
    };
    %enddef

    specialize_std_list(bool);
    specialize_std_list(char);
    specialize_std_list(int);
    specialize_std_list(short);
    specialize_std_list(long);
    specialize_std_list(unsigned char);
    specialize_std_list(unsigned int);
    specialize_std_list(unsigned short);
    specialize_std_list(unsigned long);
    specialize_std_list(float);
    specialize_std_list(double);
    specialize_std_list(std::string);

}

