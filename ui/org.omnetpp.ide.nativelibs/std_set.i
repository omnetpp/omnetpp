//
// SWIG typemaps for std::set
// Andras Varga
// Aug. 2005
//
// Common implementation

%include std_common.i
%include exception.i

%exception std::set::del  {
    try {
        $action
    } catch (std::out_of_range& e) {
        SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, const_cast<char*>(e.what()));
        return $null;
    }
}

// ------------------------------------------------------------------------
// std::set
// ------------------------------------------------------------------------

%{
#include <set>
#include <algorithm>
#include <stdexcept>
%}

// exported class

namespace std {

    template<class T> class set {
        // add typemaps here
      public:
        set();
        set(const set<T> &);

        unsigned int size() const;
        bool empty() const;
        void clear();
        %extend {
            void insert(const T& key) {
                self->insert(key);
            }
            void del(const T& key) {
                std::set<T>::iterator i = self->find(key);
                if (i != self->end())
                    self->erase(i);
                else
                    throw std::out_of_range("key not found");
            }
            bool has_key(const T& key) {
                std::set<T>::iterator i = self->find(key);
                return i != self->end();
            }
        }
    };

    // specializations for built-ins

    %define specialize_std_set(T)

    template<> class set<T> {
        // add typemaps here
      public:
        set();
        set(const set<T> &);

        unsigned int size() const;
        bool empty() const;
        void clear();
        %extend {
            void insert(T key) {
                self->insert(key);
            }
            void del(T key) {
                std::set<T>::iterator i = self->find(key);
                if (i != self->end())
                    self->erase(i);
                else
                    throw std::out_of_range("key not found");
            }
            bool has_key(T key) {
                std::set<T>::iterator i = self->find(key);
                return i != self->end();
            }
        }
    };


    %enddef

    // add specializations here

    specialize_std_set(bool);
    specialize_std_set(char);
    specialize_std_set(int);
    specialize_std_set(short);
    specialize_std_set(long);
    specialize_std_set(unsigned char);
    specialize_std_set(unsigned int);
    specialize_std_set(unsigned short);
    specialize_std_set(unsigned long);
    specialize_std_set(float);
    specialize_std_set(double);
    specialize_std_set(std::string);
}
