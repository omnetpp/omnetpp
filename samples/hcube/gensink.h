// file: gensink.h

#include <typeinfo>

/*
const char *opp_typename(const std::type_info& t)
{
    const char *s = t.name();

    // correct gcc 2.9x bug: it prepends the type name with its length
    while (*s>='0' && *s<='9') s++;

    // and MSVC prepends "class "...
    if (s[0]=='c' && s[1]=='l' && s[2]=='a' && s[3]=='s' && s[4]=='s' && s[5]==' ') s+=6;
    return s;
}

template<class T>
T check_and_cast(cObject *p)
{
    T tmp = dynamic_cast<T>(p);
    if (!tmp)
        throw new cException("Cannot cast (%s *)%s to type %s",p->className(),p->fullPath(),opp_typename(typeid(T)));
    return tmp;
}
*/

class HCGenerator : public cSimpleModule
{
       Module_Class_Members(HCGenerator,cSimpleModule,16384)
       virtual void activity();
};

class HCSink : public cSimpleModule
{
       Module_Class_Members(HCSink,cSimpleModule,16384)
       virtual void activity();
};
