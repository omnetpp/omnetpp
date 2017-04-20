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

#ifndef __FIGUREAPI_BASICS_H_
#define __FIGUREAPI_BASICS_H_

#include <omnetpp.h>
#include "TestBase.h"

using namespace omnetpp;

class Dummy : public cSimpleModule
{
    virtual void initialize() override {}
    virtual void handleMessage(cMessage *msg) override {}
};

class Visible : public TestBase
{
    cImageFigure *image;
  protected:
    virtual std::string init();
    virtual std::string onTick(int tick);
};

class AddRemove : public TestBase
{
    cImageFigure *image;
  protected:
    virtual std::string init();
    virtual std::string onTick(int tick);
};

class ZIndex : public TestBase
{
    cTextFigure *texts[5];
    cRectangleFigure *rectangle;
  protected:
    virtual std::string init();
    virtual std::string onTick(int tick);
};

class Tags : public TestBase
{
    cRectangleFigure *A, *B;
  protected:
    virtual std::string init();
    virtual std::string onTick(int tick);
};

class Transform : public TestBase
{
    cGroupFigure *grid;
    void addFigures(bool bottom);
  protected:
    virtual std::string init();
    virtual std::string onTick(int tick) { return ""; }
    virtual std::string onFrame(int tick, double t) const;
};

class TooltipFigure;

class Misc : public TestBase
{
    TooltipFigure *N, *A, *B;
    cModule *a, *b;
  protected:
    virtual std::string init();
    virtual std::string onTick(int tick);
};


#endif
