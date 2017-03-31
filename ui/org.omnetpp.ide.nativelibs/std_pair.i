//
// SWIG typemaps for std::pair
// Andras Varga
// Aug. 2005
//
// Common implementation

%include std_common.i
%include exception.i

// ------------------------------------------------------------------------
// std::pair
// ------------------------------------------------------------------------

%{
#include <utility>
%}

// exported class

namespace std {

    template<class K, class V> class pair {
      public:
        K first;
        V second;
    };

    // specializations for built-ins

    %define specialize_std_pair(K,V)

    template<> class pair<K,V> {
      public:
        K first;
        V second;
    };

    %enddef

    // add specializations here

    specialize_std_pair(std::string, std::string);
}
