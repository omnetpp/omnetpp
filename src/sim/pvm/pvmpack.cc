//=========================================================================
//
// PVMPACK.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Zoltan Vass, 1996
//
//   Contents:
//      The definition of the function which stores an object
//      in the PVM sender buffer or restores them from the receiver buffer.
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <pvm3.h>

#include "omnetpp.h"
#include "pvmmod.h"

//=========================================================================
// Helper functions to pack or unpack an object, referenced by its pointer
//=========================================================================

int notnull(void *ptr, int& err)
{
        char flag=0;
        if (ptr) flag=1;
        err=err||pvm_pkbyte(&flag,1,1);
        return flag;
}

int chkflag(int& err)
{
        char flag;
        err=err||pvm_upkbyte(&flag,1,1);
        return flag;
}

//=========================================================================
// Functions that handle strings
//=========================================================================
int pack_str(const char *str)
{
        int err=0,len=0;
        if (str) len=strlen(str);
        err=pvm_pkint(&len,1,1);
        if (len) err=err||pvm_pkstr(CONST_CAST(str));
        return err;
}

char *upack_str(int& err)
{
        int len=0;
        char * tmp;
        err=err||pvm_upkint(&len,1,1);
        if (len)
         {
         tmp=new char[len+1];
         err=err||pvm_upkstr(tmp);
         return tmp;
         }
        return NULL;
}

//=========================================================================
// Function that unpacks an object
//=========================================================================
cObject *upack_object(int& err)
{
        char *clname = upack_str(err);
        if (err) {delete clname; return NO(cObject);}
        cObject *obj = createOne(clname);
        obj->netUnpack();
        delete clname;
        return obj;
}

//=========================================================================
// Methods of the different classes defined in the simulation library.
// First the store, next the restore function for a class.
// In the unpack functions, we use an utility, createOne(char *), to create a
// new object of the given class. This is defined in the ctype.cc.
// Each function returns an acccumulated error flag, that shows whether the
// PVM pack and unpack fails.
//=========================================================================

int cObject::netPack()
{
        int err=0;
        err=pack_str(className());
        err=err||pack_str(namestr);
        return err;
}

int  cObject::netUnpack()
{
        int err=0;
        // upacking className() string already done by upack_object() at this point
        namestr=upack_str(err);
        return err;
}


int cPar::netPack()
{
        int err=0;
        err|=cObject::netPack();
        err|=pvm_pkbyte(&typechar,1,1);
        err|=pvm_pkbyte((char*)&inputflag,1,1);
        err|=pvm_pkbyte((char*)&changedflag,1,1);
        err|=pack_str(promptstr);
        cFunctionType *ff;
        switch (typechar)
                {
                case 'S':
                        err|=pvm_pkbyte((char*)&(ls.sht),1,1);
                        if (notnull(ls.str,err)) err|=pack_str(ls.str);
                        break;
                case 'C':
                        err|=pvm_pkbyte((char*)&(ss.sht),1,1);
                        if (notnull(ss.str,err)) err|=pack_str(ss.str);
                        break;
                case 'L':
                        err|=pvm_pklong(&(lng.val),1,1);
                        break;
                case 'D':
                        err|=pvm_pkdouble(&(dbl.val),1,1);
                        break;
                case 'F':
                        ff = findfunctionbyptr(func.f);
                        if (ff==NULL)
                            {opp_error("cPar::netPack(): cannot transmit unregistered function");return 1;}
                        err|=pack_str(ff->name());
                        err|=pvm_pkint(&(func.argc),1,1);
                        err|=pvm_pkdouble(&(func.p1),1,1);
                        err|=pvm_pkdouble(&(func.p2),1,1);
                        err|=pvm_pkdouble(&(func.p3),1,1);
                        break;
                case 'T':
                        if (dtr.res && dtr.res->owner()!=this)
                            {opp_error("cPar::netPack(): cannot transmit pointer to \"external\" object");return 1;}
                        if (notnull(dtr.res,err)) err|=dtr.res->netPack();
                        break;
                case 'I':
                        opp_error("cPar::netPack(): transmitting indirect values (type 'I') not supported");
                        return 1;
                case 'X':
                        opp_error("cPar::netPack(): transmitting expressions (type 'X') not supported");
                        return 1; // not implemented, because there are functions and pointers in it.
                case 'P':
                        opp_error("cPar::netPack(): cannot transmit pointer to unknown data structure (type 'P')");
                        return 1;
                case 'O':
                        if (obj.obj && obj.obj->owner()!=this)
                            {opp_error("cPar::netPack(): cannot transmit pointer to \"external\" object");return 1;}
                        if (notnull(obj.obj,err)) err|=obj.obj->netPack();
                        break;
                }
        return err;
}

int cPar::netUnpack()
{
        char *funcname;
        int err=0;
        err|=cObject::netUnpack();
        err|=pvm_upkbyte(&typechar,1,1);
        err|=pvm_upkbyte((char*)&inputflag,1,1);
        err|=pvm_upkbyte((char*)&changedflag,1,1);
        promptstr=upack_str(err);
        char *tip;
        cFunctionType *ff;
        switch (typechar)
                {
                case 'S':
                        err|=pvm_upkbyte((char*)&(ls.sht),1,1);
                        ls.str=upack_str(err);

                case 'C':
                        err|=pvm_upkbyte((char*)&(ss.sht),1,1);
                        tip=upack_str(err);
                        if (tip) strcpy(ss.str,tip);
                        break;
                case 'L':
                        err|=pvm_upklong(&(lng.val),1,1);
                        break;
                case 'D':
                        err|=pvm_upkdouble(&(dbl.val),1,1);
                        break;
                case 'F':
                        funcname=upack_str(err);
                        ff = findFunction(funcname);
                        if (ff==NULL) {
                             opp_error("cPar::netUnpack(): transmitted function `%s' not registered here",funcname);
                             delete funcname;
                             return 1;
                             }
                        func.f = ff->f;
                        err|=pvm_upkdouble(&(func.p1),1,1);
                        err|=pvm_upkdouble(&(func.p2),1,1);
                        err|=pvm_upkdouble(&(func.p3),1,1);
                        delete funcname;
                        break;
                case 'T':
                        if (!chkflag(err))
                            dtr.res=NULL;
                        else
                            take( dtr.res = (cStatistic *)upack_object(err) );
                        break;
                case 'I':
                case 'X':
                case 'P':
                        opp_error("cPar::netUnpack(): WHAT???");
                        return -1;  // sending of 'X' not implemented
                case 'O':
                        if (!chkflag(err))
                            obj.obj=NULL;
                        else
                            take( obj.obj = upack_object(err) );
                        break;
                }
        return err;
}

int cBag::netPack()
{
        int err=0;
        err|=pvm_pkint(&elemsize,1,1);
        err|=pvm_pkint(&size,1,1);
        err|=pvm_pkint(&delta,1,1);
        err|=pvm_pkint(&lastused,1,1);
        err|=pvm_pkint(&firstfree,1,1);
        err|=pvm_pkbyte(vect,size*(sizeof(bool)+elemsize),1);
        return err;
}

int cBag::netUnpack()
{
        int err=0;
        err|=pvm_upkint(&elemsize,1,1);
        err|=pvm_upkint(&size,1,1);
        err|=pvm_upkint(&delta,1,1);
        err|=pvm_upkint(&lastused,1,1);
        err|=pvm_upkint(&firstfree,1,1);
        err|=pvm_upkbyte(vect,size*(sizeof(bool)+elemsize),1);
        return err;
}

int cQueue::netPack()
{
        int err=0;
        err|=cObject::netPack();
        err|=pvm_pkint(&n,1,1);
        err|=pvm_pkbyte((char*)&asc,1,1);
        for (sQElem * pt=headp;pt!=NULL;pt=pt->next)
            if (notnull(pt->obj,err)) {
                if (pt->obj->owner()!=this)
                    {opp_error("cQueue::netPack(): cannot transmit pointer"
                                      " to \"external\" object");return 1;}
                err|=pt->obj->netPack();
                }
        return err;
}

int cQueue::netUnpack()
{
        int err=0;
        err=cObject::netUnpack();
        err|=pvm_upkint(&n,1,1);
        err|=pvm_upkbyte((char*)&asc,1,1);
        if (n)  {
                headp = new sQElem;
                sQElem * pt1=headp;
                sQElem * pt2;
                pt1->prev=NULL;
                take( pt1->obj = upack_object(err) );
                for (int i=1;i<n;i++) {
                        pt2=pt1;
                        pt1->next = new sQElem;
                        pt1=pt1->next;
                        pt1->prev=pt2;
                        take( pt1->obj = upack_object(err) );
                        }
                pt1->next=NULL;
                tailp=pt1;
                }
        return err;
}

int cArray::netPack()
{
        int err=0;
        err|=cObject::netPack();
        err=err || pvm_pkint(&size,1,1);
        err=err || pvm_pkint(&delta,1,1);
        err=err || pvm_pkint(&firstfree,1,1);
        err=err || pvm_pkint(&last,1,1);
        for (int i=0;i<=last;i++)
            if (notnull(vect[i],err)) {
                if (vect[i]->owner()!=this)
                    {opp_error("cArray::netPack(): cannot transmit pointer"
                                      " to \"external\" object");return 1;}
                err|=vect[i]->netPack();
                }
        return err;
}

int cArray::netUnpack()
{
        int err=0;
        err=cObject::netUnpack();
        delete vect;
        err=err || pvm_upkint(&size,1,1);
        err=err || pvm_upkint(&delta,1,1);
        err=err || pvm_upkint(&firstfree,1,1);
        err=err || pvm_upkint(&last,1,1);
        vect = new cObject *[size];
        for (int i=0;i<=last;i++)
                if (!chkflag(err))
                        vect[i] = NULL;
                else
                        take( vect[i] = upack_object(err) );
        return err;
}

int cLinkedList::netPack()
{
        opp_error("(%s)%s: netPack() not possible -- don't know how to pack an item",className(),fullName());
        return 0;
}

int cLinkedList::netUnpack()
{
        opp_error("(%s)%s: netUnpack() not possible",className(),fullName());
        return 0;
}

int cMessage::netPack()
{
        int err=0;
        err|=cObject::netPack();
        err|=pvm_pkint(&msgkind,1,1);
        err|=pvm_pkint(&prior,1,1);
        err|=pvm_pklong(&len,1,1);
        err|=pvm_pkdouble(&tstamp,1,1);
        err|=pvm_pkbyte((char*)&error,1,1);
        err|=pvm_pkint(&frommod,1,1);
        err|=pvm_pkint(&fromgate,1,1);
        err|=pvm_pkint(&tomod,1,1);
        err|=pvm_pkint(&togate,1,1);
        err|=pvm_pkdouble(&sent,1,1);
        err|=pvm_pkdouble(&delivd,1,1);
        if (notnull(parlistp,err)) parlistp->netPack();
        return err;
}

int cMessage::netUnpack()
{
        int err=0;
        err=cObject::netUnpack();
        err|=pvm_upkint(&msgkind,1,1);
        err|=pvm_upkint(&prior,1,1);
        err|=pvm_upklong(&len,1,1);
        err|=pvm_upkdouble(&tstamp,1,1);
        err|=pvm_upkbyte((char*)&error,1,1);
        err|=pvm_upkint(&frommod,1,1);
        err|=pvm_upkint(&fromgate,1,1);
        err|=pvm_upkint(&tomod,1,1);
        err|=pvm_upkint(&togate,1,1);
        err|=pvm_upkdouble(&sent,1,1);
        err|=pvm_upkdouble(&delivd,1,1);
        if (chkflag(err))
            take(parlistp = (cArray *)upack_object(err));
        return err;
}


int cPacket::netPack()
{
        int err=0;
        err|=cMessage::netPack();
        err|=pvm_pkshort(&_protocol,1,1);
        err|=pvm_pkshort(&_pdu,1,1);
        return err;
}

int cPacket::netUnpack()
{
        int err=0;
        err=cMessage::netUnpack();
        err|=pvm_upkshort(&_protocol,1,1);
        err|=pvm_upkshort(&_pdu,1,1);
        return err;
}


int cTransientDetection::netPack()
{
        int err=0;
        return err||cObject::netPack();
}

int cTransientDetection::netUnpack()
{
        return cObject::netUnpack();
}

int cAccuracyDetection::netPack()
{
        int err=0;
        return err||cObject::netPack();
}

int cAccuracyDetection::netUnpack()
{
        return cObject::netUnpack();
}


int cStatistic::netPack()
{
        int err=0;
        err=cObject::netPack();
        err|=pvm_pkint(&genk,1,1);
        if (notnull(td,err)) err|=td->netPack();
        if (notnull(ra,err)) err|=ra->netPack();
        return err;
}

int cStatistic::netUnpack()
{
        int err=0;
        err=cObject::netUnpack();
        err|=pvm_upkint(&genk,1,1);
        if (chkflag(err))
                take( td = (cTransientDetection *)upack_object(err) );
        if (chkflag(err))
                take( ra = (cAccuracyDetection *)upack_object(err) );
        return err;
}


int cStdDev::netPack()
{
        int err=0;
        err|=cStatistic::netPack();
        err|=pvm_pklong(&num_samples,1,1);
        err|=pvm_pkdouble(&min_samples,1,1);
        err|=pvm_pkdouble(&max_samples,1,1);
        err|=pvm_pkdouble(&sum_samples,1,1);
        err|=pvm_pkdouble(&sqrsum_samples,1,1);
        return err;
}

int cStdDev::netUnpack()
{
        int err=0;
        err=cStatistic::netUnpack();
        err|=pvm_upklong(&num_samples,1,1);
        err|=pvm_upkdouble(&min_samples,1,1);
        err|=pvm_upkdouble(&max_samples,1,1);
        err|=pvm_upkdouble(&sum_samples,1,1);
        err|=pvm_upkdouble(&sqrsum_samples,1,1);
        return err;
}


int cWeightedStdDev::netPack()
{
        int err=0;
        err|=cStdDev::netPack();
        err|=pvm_pkdouble(&sum_weights,1,1);
        return err;
}

int cWeightedStdDev::netUnpack()
{
        int err=0;
        err=cStdDev::netUnpack();
        err|=pvm_upkdouble(&sum_weights,1,1);
        return err;
}


int cDensityEstBase::netPack()
{
        int err=0;
        err|=cStdDev::netPack();
        err|=pvm_pkdouble(&rangemin,1,1);
        err|=pvm_pkdouble(&rangemax,1,1);
        err|=pvm_pklong(&num_firstvals,1,1);
        err|=pvm_pkulong(&cell_under,1,1);
        err|=pvm_pkulong(&cell_over,1,1);
        err|=pvm_pkdouble(&range_ext_factor,1,1);
        err|=pvm_pkint(&range_mode,1,1);
        int t=transfd; err|=pvm_pkint(&t,1,1);

        if (notnull(firstvals,err))
           err|=pvm_pkdouble(firstvals,num_firstvals,1);
        return err;
}

int cDensityEstBase::netUnpack()
{
        int err=0;
        err=cStdDev::netUnpack();
        err|=pvm_upkdouble(&rangemin,1,1);
        err|=pvm_upkdouble(&rangemax,1,1);
        err|=pvm_upklong(&num_firstvals,1,1);
        err|=pvm_upkulong(&cell_under,1,1);
        err|=pvm_upkulong(&cell_over,1,1);
        err|=pvm_upkdouble(&range_ext_factor,1,1);
        err|=pvm_upkint(&range_mode,1,1);
        int t; err|=pvm_upkint(&t,1,1); transfd=(t!=0);
        if (chkflag(err))
           err|=pvm_upkdouble(firstvals=new double[num_firstvals],num_firstvals,1);
        return err;
}

int cHistogramBase::netPack()
{
        int err=0;
        err|=cDensityEstBase::netPack();
        err|=pvm_pkint(&num_cells,1,1);
        if (notnull(cellv,err))
           err|=pvm_pkuint(cellv,num_cells,1);
        return err;
}

int cHistogramBase::netUnpack()
{
        int err=0;
        err=cDensityEstBase::netUnpack();
        err|=pvm_upkint(&num_cells,1,1);
        if (chkflag(err))
           err|=pvm_upkuint(cellv=new unsigned[num_cells],num_cells,1);
        return err;
}

int cEqdHistogramBase::netPack()
{
        int err=0;
        err|=cHistogramBase::netPack();
        err|=pvm_pkdouble(&cellsize,1,1);
        return err;
}

int cEqdHistogramBase::netUnpack()
{
        int err=0;
        err=cHistogramBase::netUnpack();
        err|=pvm_upkdouble(&cellsize,1,1);
        return err;
}

int cLongHistogram::netPack()
{
        int err=0;
        err=cEqdHistogramBase::netPack();
        return err;
}

int cLongHistogram::netUnpack()
{
        int err=0;
        err=cEqdHistogramBase::netUnpack();
        return err;
}

int cDoubleHistogram::netPack()
{
        int err=0;
        err=cEqdHistogramBase::netPack();
        return err;
}

int cDoubleHistogram::netUnpack()
{
        int err=0;
        err=cEqdHistogramBase::netUnpack();
        return err;
}

int cVarHistogram::netPack()
{
        int err=0;
        err|=cHistogramBase::netPack();
        err|=pvm_pkint(&max_num_cells,1,1);
        if (notnull(cellv,err))
           err|=pvm_pkuint(cellv,max_num_cells,1);
        if (notnull(bin_bounds,err))
           err|=pvm_pkdouble(bin_bounds,max_num_cells+1,1);
        return err;
}

int cVarHistogram::netUnpack()
{
        int err=0;
        err=cHistogramBase::netUnpack();
        err|=pvm_upkint(&max_num_cells,1,1);
        delete cellv; cellv=NULL; // must recreate with different size
        if (chkflag(err))
           err|=pvm_upkuint(cellv=new unsigned[max_num_cells],max_num_cells,1);
        if (chkflag(err))
           err|=pvm_upkdouble(bin_bounds=new double[max_num_cells+1],max_num_cells+1,1);
        return err;
}


int cPSquare::netPack()
{
        int err=0;
        err|=cDensityEstBase::netPack();
        err|=pvm_pkint(&numcells,1,1);
        err|=pvm_pklong(&numobs,1,1);
        if (notnull(n,err))
           err|=pvm_pkint(n,numcells+2,1);
        if (notnull(q,err))
           err|=pvm_pkdouble(q,numcells+2,1);
        return err;
}

int cPSquare::netUnpack()
{
        int err=0;
        err=cDensityEstBase::netUnpack();
        err|=pvm_upkint(&numcells,1,1);
        err|=pvm_upklong(&numobs,1,1);
        if (chkflag(err))
           err|=pvm_upkint(n=new int[numcells+2],numcells+2,1);
        if (chkflag(err))
           err|=pvm_upkdouble(q=new double[numcells+2],numcells+2,1);
        return err;
}


int cKSplit::netPack()
{
        int err=0;
        err|=cDensityEstBase::netPack();

        opp_error("(%s)%s: netPack() not implemented",className(), fullName());
        return err;
}

int cKSplit::netUnpack()
{
        int err=0;
        err=cDensityEstBase::netUnpack();

        opp_error("(%s)%s: netUnpack() not implemented",className(),fullName());
        return err;
}


int cTopology::netPack()
{
        opp_error("(%s)%s: netPack() not implemented",className(), fullName());
        return 0;
}

int cTopology::netUnpack()
{
        opp_error("(%s)%s: netUnpack() not implemented",className(),fullName());
        return 0;
}

int cFSM::netPack()
{
        opp_error("(%s)%s: netPack() not implemented",className(), fullName());
        return 0;
}

int cFSM::netUnpack()
{
        opp_error("(%s)%s: netUnpack() not implemented",className(),fullName());
        return 0;
}

