//=========================================================================
//  STARTREEEMBEDDING.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __STARTREEEMBEDDING_H_
#define __STARTREEEMBEDDING_H_

#include "vector"
#include "geometry.h"
#include "graphcomponent.h"

class StarTreeEmbedding
{
    public:
	    double nodeSpacing;

    private:
        GraphComponent *graphComponent;

    public:
	    StarTreeEmbedding(GraphComponent *graphComponent, double nodeSpacing);
	    void embed();

    private:
	    void calculateCenter();
	    void calculateCenterRecursive(Vertex *vertex);
	    void rotateCenter();
	    void rotateCenterRecursive(Vertex *vertex);
	    void calculatePosition();
	    void calculatePositionRecursive(Vertex *vertex, Pt pt);
};

#endif
