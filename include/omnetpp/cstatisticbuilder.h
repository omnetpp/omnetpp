//==========================================================================
//  CSTATISTICBUILDER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CSTATISTICBUILDER_H
#define __OMNETPP_CSTATISTICBUILDER_H

#include "omnetpp/clistener.h"
#include "omnetpp/cproperty.h"

namespace omnetpp {

extern const char *PROPKEY_STATISTIC_SOURCE;
extern const char *PROPKEY_STATISTIC_RECORD;
extern const char *PROPKEY_STATISTIC_CHECKSIGNALS;

class cConfiguration;
struct SignalSource;

/**
 * Utility class for configuring result recording from @statistic properties.
 */
class SIM_API cStatisticBuilder
{
    public:
        enum TristateBool {TRISTATE_TRUE=1, TRISTATE_FALSE=0, TRISTATE_DEFAULT=-1};  // for internal use only

    private:
        cConfiguration *config;

    public:
        cStatisticBuilder(cConfiguration *config) : config(config) {}

        /**
         * Configures statistic recording for the component, using @statistic
         * properties.
         */
        void addResultRecorders(cComponent *component);

        /**
         * Configures statistic recording for the given signal and statistic name,
         * using the given property for details..
         */
        void addResultRecorders(cComponent *component, simsignal_t signal, const char *statisticName, cProperty *statisticTemplateProperty);

    protected:
        // Utility function: adds result recording listeners for the given signal (if it's non-null) or for the given @statistic property.
        // If signal is specified, it will override the source= key in statisticProperty.
        // The index of statisticProperty is ignored; statisticName will be used as name of the statistic instead.
        void doAddResultRecorders(cComponent *component, std::string& componentFullPath, const char *statisticName, cProperty *statisticProperty, simsignal_t signal=SIMSIGNAL_NULL);

        // Utility functions for addResultRecorders()
        std::vector<std::string> extractRecorderList(const char *modesOption, cProperty *statisticProperty);
        SignalSource doStatisticSource(cComponent *component, cProperty *statisticProperty, const char *statisticName, const char *sourceSpec, TristateBool checkSignalDecl, bool needWarmupFilter);
        void doResultRecorder(const SignalSource& source, const char *mode, cComponent *component, const char *statisticName, cProperty *attrsProperty);
        TristateBool parseTristateBool(const char *s, const char *what);
};

}  // namespace omnetpp

#endif


