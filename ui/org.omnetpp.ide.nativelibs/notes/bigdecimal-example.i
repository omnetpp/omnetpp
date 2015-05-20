%module bigdec;

%typemap(jstype)  omnetpp::common::BigDecimal "org.omnetpp.common.engine.BigDecimal";
%typemap(javain)  omnetpp::common::BigDecimal "org.omnetpp.common.engine.BigDecimal.getCPtr($javainput)"
%typemap(javaout) omnetpp::common::BigDecimal {  blah... }
%typemap(in)      omnetpp::common::BigDecimal ($&1_type argp) %{ blah... %}
%typemap(out)     omnetpp::common::BigDecimal %{ blah... %}

namespace omnetpp { namespace scave {

//using omnetpp::common::BigDecimal; <----- not good enough! need typedef instead
typedef omnetpp::common::BigDecimal BigDecimal;

class DataSorter {
public:
    BigDecimal getData();
    void setData(BigDecimal x);
};

} } // omnetpp::scave