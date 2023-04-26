
cplusplus {{
#include "problematic.h"
}}

struct DummyStruct {
    @existingClass;
}

class DummyClass extends cOwnedObject {
    @existingClass;
}

class Problematic extends cOwnedObject {
    @existingClass;

    int testGetter @getter(throwingGetter) @readonly;
    int testSetter @setter(throwingSetter) @getter(dummyGetter);

    DummyStruct testStructToString @toString(throwingStructToString) @readonly @getter(dummyStructGetter);

    int testSizeGetter[] @sizeGetter(throwingSizeGetter) @getter(dummyElementGetter) @readonly;

    int testElementGetter[] @getter(throwingElementGetter) @sizeGetter(dummySizeGetter) @readonly;
    DummyStruct *testStructelementPointerGetter[] @getter(throwingStructElementPointerGetter) @sizeGetter(dummySizeGetter) @readonly;
    DummyClass *testClassElementPointerGetter[] @getter(throwingClassElementPointerGetter) @sizeGetter(dummySizeGetter) @readonly;

    // Untested because unused by Qtenv GOI: @getterForUpdate, @str, @toValue, etc.
}