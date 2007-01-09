%{
#include "node.h"
#include "dataflowmanager.h"
#include "nodetype.h"
#include "nodetyperegistry.h"
#include "arraybuilder.h"
#include "xyarray.h"
//#include "ivectorfilereader.h"
#include "indexedvectorfile.h"
#include "vectorfileindexer.h"
%}

%include "std_common.i"
%include "std_string.i"
%include "std_map.i"
%include "std_vector.i"
%include "std_list.i"    // our custom version

namespace std {

   //
   // Without the following macro (defined in std_map.i in the swig library),
   // get() returns string *pointer* (because of T& retval); this macro defines
   // get() to return T so it'll be just string. Same with NodeType*.
   //
   specialize_std_map_on_both(std::string,,,,std::string,,,);
   specialize_std_vector(NodeType*);

   // generate needed template specializations
   %template(StringMap) map<string,string>;
   %template(NodeTypeVector) vector<NodeType*>;
};

//
// We publish very little of the Node and Port classes into Java,
// just enough to assemble the data-flow engine. We hide Channel and
// Datum altogether.
//

class NodeType;
class XYArray;

class Port
{
  private:
    Port(); // prevent generating a default public one
    ~Port();
  public:
    Node *node();
};

class Node
{
  private:
    Node(); // prevent generating a default public one
    ~Node();
  public:
    NodeType *nodeType() const;
    %extend {
        XYArray *getArray() {
            ArrayBuilderNode *abNode = dynamic_cast<ArrayBuilderNode *>(self);
            return abNode ? abNode->getArray() : NULL;
        }
    }
};

struct OutputVectorEntry
{
    const long serial;
    const double simtime;
    const double value;

    OutputVectorEntry(long serial, double simtime, double value);
};


class IndexedVectorFileReader
{
    public:
        explicit IndexedVectorFileReader(const char* filename, int vectorId);
        virtual ~IndexedVectorFileReader();

	int getNumberOfEntries();
        OutputVectorEntry *getEntryBySerial(long serial);
};


%ignore DataflowManager::addNode; // it's included in NodeType::create()

%ignore XYArray::XYArray;

//
// NodeType, NodeTypeRegistry and DataflowManager are published in their entirety
// to Java.
//
%include "nodetype.h"
%include "nodetyperegistry.h"
%include "dataflowmanager.h"
%include "xyarray.h"
%include "vectorfileindexer.h"
// %include "ivectorfilereader.h"
//%include "indexedvectorfile.h"  XXX commented out for now, wrap.cc didn't compile (Andras)


