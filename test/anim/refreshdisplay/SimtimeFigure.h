#ifndef SIMTIMEFIGURE_H
#define SIMTIMEFIGURE_H

#include <omnetpp.h>

using namespace omnetpp;

class SimtimeFigure : public cTextFigure
{
    private:
        int refreshCount = 0;
        bool throwExceptionInRefresh = false;
        bool exceptionThrown = false;
    public:
        SimtimeFigure();
        void setThrowExceptionInRefresh(bool b) {throwExceptionInRefresh = b;}
        void throwException();
        virtual void refreshDisplay() override;
};

#endif
