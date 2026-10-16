
cplusplus {{
    std::string printGeneratedStruct(const GeneratedStruct& gs);
}}

struct GeneratedStruct
{
    @toString(printGeneratedStruct($));
    int foo;
    double bar;
    string baz;
}

class GeneratedClass extends cObject
{
    int one;
    double two;
    string three;
}

message GeneratedMessage
{
    int yksi;
    double kaksi;
    string kolme;
}

packet GeneratedPacket
{
    int eins;
    double zwei;
    string drei;
}

//
// Types for the Printability module
//

// has operator<< (defined below)
struct PrintableStruct
{
    int a;
}

cplusplus {{
inline std::ostream& operator<<(std::ostream& os, const PrintableStruct& s) { return os << "PrintableStruct a=" << s.a << " (printed by op<<)"; }
}}

// no @toString, no operator<<
struct PlainStruct
{
    int b;
}

cplusplus {{
// no operator<<, no descriptor
struct OpaqueThing { int c = 0; };
}}

class OpaqueThing
{
    @existingClass;
    @opaque;
}

message PrintabilityMessage
{
    GeneratedStruct withToString;   // displayed via @toString
    PrintableStruct printable;      // displayed via operator<<
    PlainStruct plain;              // compound: "" (expand it to see the fields)
    OpaqueThing opaque;             // not compound: "(unprintable)"
    PrintableStruct *printablePtr = nullptr;
    PlainStruct *plainPtr = nullptr;
}
