// The default Java wrapper of std::map was changed in
// SWIG 4.0, so some functions in it were renamed.
// See: https://github.com/swig/swig/pull/1356
// This file adds back the old names, using the new
// ones to implement them, as recommended in:
// http://www.swig.org/Release/CHANGES

#if SWIG_VERSION >= 0x040000

%include <std_map.i>

%{
#include <map>
%}

%extend std::map {
    %proxycode %{
    // Old API
    public boolean empty() {
        return isEmpty();
    }
    public void set($typemap(jboxtype, K) key, $typemap(jboxtype, T) x) {
        put(key, x);
    }
    public void del($typemap(jboxtype, K) key) {
        remove(key);
    }
    public boolean has_key($typemap(jboxtype, K) key) {
        return containsKey(key);
    }
    %}
}

#endif // SWIG_VERSION >= 0x040000