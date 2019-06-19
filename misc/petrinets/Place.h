//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef __PLACE_H__
#define __PLACE_H__

#include <omnetpp.h>
#include "IPlace.h"
#include "ITransition.h"

using namespace omnetpp;

/**
 * Implements a Petri Net place
 */
class Place : public cSimpleModule, public IPlace
{
  protected:
    int numTokens;

  protected:
    virtual int numInitStages() const {return 2;}
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);

    // helpers
    virtual void numTokensChanged();
    virtual ITransition *getOutputTransition(int i);

  public:
    // IPlace methods:
    virtual int getNumTokens();
    virtual void addTokens(int n);
    virtual void removeTokens(int n);
};

#endif
