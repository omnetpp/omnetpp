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

#ifndef __TEXTHALO_TESTER_H_
#define __TEXTHALO_TESTER_H_

#include <omnetpp.h>

using namespace omnetpp;

class Tester : public cSimpleModule
{
    bool wave;
    std::vector<cAbstractTextFigure *> figures;
    cRectangleFigure *dark, *light;

  protected:
    std::string makeString(int length);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void refreshDisplay() const;
};

#endif
