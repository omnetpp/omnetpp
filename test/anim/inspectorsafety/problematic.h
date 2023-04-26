
#ifndef PROBLEMATIC_H
#define PROBLEMATIC_H

#include <omnetpp.h>

static void setDispStr(omnetpp::cModule *mod) {
    std::string ds = "bgb=700,500;bgt=100,200,";

    bool testComplete = true;
    for (int i = 0; i < mod->getNumParams(); ++i) {
        omnetpp::cPar &par = mod->par(i);
        ds += std::string("\n") + par.getName() + "\t" + par.str();

        if (!par.boolValue())
            testComplete = false;
    }

    ds += "\n\nTest complete?\t";
    ds += testComplete ? "YES,green" : "NO,red";

    mod->getDisplayString().parse(ds.c_str());
}

static void setPar(const char *parname) {
    omnetpp::cSimulation *sim = omnetpp::cSimulation::getActiveSimulation();
    omnetpp::cModule *network = sim->getSystemModule();
    network->par(parname).setBoolValue(true);
    setDispStr(network);
}

struct DummyStruct {
    std::string throwingStructToString() const {
        setPar("throwingStructToStringCalled");
        throw omnetpp::cRuntimeError("Exception from DummyStruct @toString");
    }
};

class DummyClass : public omnetpp::cOwnedObject {

};


class Problematic : public omnetpp::cOwnedObject
{
    DummyStruct dummyStruct;
public:

    Problematic() {
        omnetpp::cSimulation *sim = omnetpp::cSimulation::getActiveSimulation();
        omnetpp::cModule *network = sim->getSystemModule();
        omnetpp::cCanvas *canvas = network->getCanvas();

        omnetpp::cPanelFigure *panel = new omnetpp::cPanelFigure();
        omnetpp::cLabelFigure *label = new omnetpp::cLabelFigure();
        label->setText("To perform the test:\n"
            " - Click HERE to select the problematic object.\n"
            " - In the object inspector, in Flat mode, open all the array fields.\n"
            " - Try to change the value of the `testSetter` field,\n"
            "   (there should be a popup about it throwing an error).\n"
            " - Press 'Re-layout' on the module inspector to see the checklist below.\n"
            " - The popup about an exception asking for debugging should never appear.\n"
            " - After the test completed, the simulation should still not be in an error state\n"
            "  (it should be able to be started, and complete immediately).");
        label->setAssociatedObject(this);
        label->setPosition(omnetpp::cFigure::Point(100, 50));

        panel->addFigure(label);
        canvas->addFigure(panel);
    }

    int dummyGetter() const { return 4; } // chosen by fair dice roll
    const DummyStruct &dummyStructGetter() { return dummyStruct;}
    int dummySizeGetter() { return 42; }
    int dummyElementGetter(int _i) { return 42; }

    int throwingGetter() const {
        setPar("throwingGetterCalled");
        throw omnetpp::cRuntimeError("Exception from @getter");
    }

    void throwingSetter(int _) const {
        setPar("throwingSetterCalled");
        throw omnetpp::cRuntimeError("Exception from @setter");
    }

    int throwingSizeGetter() const {
        setPar("throwingSizeGetterCalled");
        throw omnetpp::cRuntimeError("Exception from @sizeGetter");
    }

    int throwingElementGetter(int _i) const {
        setPar("throwingElementGetterCalled");
        throw omnetpp::cRuntimeError("Exception from element @getter");
    }

    DummyStruct *throwingStructElementPointerGetter(int _i) const {
        setPar("throwingStructElementPointerGetterCalled");
        throw omnetpp::cRuntimeError("Exception from struct element pointer @getter");
    }

    DummyClass *throwingClassElementPointerGetter(int _i) const {
        setPar("throwingClassElementPointerGetterCalled");
        throw omnetpp::cRuntimeError("Exception from class element pointer @getter");
    }
};


#endif // ifndef PROBLEMATIC_H