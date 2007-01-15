//=========================================================================
//  HEAPEMBEDDING.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __HEAPEMBEDDING_H_
#define __HEAPEMBEDDING_H_

#include "vector"
#include "geometry.h"
#include "graphcomponent.h"

class HeapEmbedding
{
    public:
	    double nodeSpacing;

    private:
        GraphComponent *graphComponent;

    public:
	    HeapEmbedding(GraphComponent *graphComponent, double nodeSpacing);
	    void embed();

    private:
        void pushPtUnlessCirclesContains(std::vector<Pt>& pts, std::vector<Cc>& circles, Pt& pt);
};

#endif