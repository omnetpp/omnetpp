//==========================================================================
//  OBJINSP.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OBJINSP_H
#define __OBJINSP_H

#include <tk.h>
#include "inspector.h"
#include "structinsp.h"
#include "omnetapp.h"

class TGenericObjectInspector : public TInspector
{
   protected:
      bool hascontentspage;
      bool focuscontentspage;
   public:
      TGenericObjectInspector(cObject *obj, int typ, const char *geom, void *dat=NULL);
      ~TGenericObjectInspector();
      void setContentsPage(bool show, bool focus) {hascontentspage = show; focuscontentspage = focus;}
      virtual void createWindow();
      virtual void update();
      virtual void writeBack();
      virtual int inspectorCommand(Tcl_Interp *interp, int argc, const char **argv);
};

//
// *** Note: the following inspectors have been replaced with TGenericObjectInspector ***
//
// class TObjInspector : public TInspector
// {
//    public:
//       TObjInspector(cObject *obj,int typ,const char *geom,void *dat=NULL);
//       ~TObjInspector();
//       virtual void createWindow();
//       virtual void update();
//       virtual void writeBack();
//       virtual int inspectorCommand(Tcl_Interp *interp, int argc, const char **argv);
// };
//
// class TContainerInspector : public TInspector
// {
//    protected:
//       bool deep;
//       char listbox[64];
//    public:
//       TContainerInspector(cObject *obj,int typ,const char *geom,void *dat=NULL);
//       virtual void createWindow();
//       virtual void update();
// };
//
// class TMessageInspector: public TInspector
// {
//    protected:
//       TStructPanel *controlinfopage;
//    public:
//       TMessageInspector(cObject *obj,int typ,const char *geom,void *dat=NULL);
//       ~TMessageInspector();
//       virtual void createWindow();
//       virtual void update();
//       virtual void writeBack();
//       virtual int inspectorCommand(Tcl_Interp *interp, int argc, const char **argv);
// };
//

class TWatchInspector: public TInspector
{
   public:
      TWatchInspector(cObject *obj,int typ,const char *geom,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
      virtual void writeBack();
};

class TParInspector: public TInspector
{
   public:
      TParInspector(cObject *obj,int typ,const char *geom,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
      virtual void writeBack();
};

#endif



