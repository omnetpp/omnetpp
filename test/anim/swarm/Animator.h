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

#ifndef __MANHATTAN_ANIMATOR_H
#define __MANHATTAN_ANIMATOR_H

#include <omnetpp.h>

using namespace omnetpp;

class Animator : public cSimpleModule
{
  protected:
    double playgroundWidth, playgroundHeight;
    std::vector<cFigure*> bees;
  protected:
    virtual void initialize();
    virtual void refreshDisplay() const;
    void setPosition(cFigure *figure, double x, double y) const;
    virtual void handleMessage(cMessage *msg);
};

#endif
