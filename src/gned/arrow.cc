//==========================================================================
//  ARROW.CC - part of
//
//                     OMNeT++/OMNEST
//
//  Implementation of
//   connection arrow positioning in module drawing
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

//
// NOTE: This file is shared between GNED and TKENV. The two copies need to be
//       kept in sync.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "arrow.h"

static void clip_line_to_rect(
            double& x1, double& y1, double x2, double y2,
            double rx1, double ry1, double rx2, double ry2)
{
   // line: (x1,y1) to (x2,y2)
   // rectangle: rx1,ry1,rx2,ry2
   //
   // adjust (x1,y1) end of the line so that it fits on the rectangle boundary
   // if (x2,y2) is inside the rectangle, do nothing

   int p1_inside = (x1>=rx1 && x1<=rx2 && y1>=ry1 && y1<=ry2);
   int p2_inside = (x2>=rx1 && x2<=rx2 && y2>=ry1 && y2<=ry2);

   // we'll clip the line to two edges of the rect: y=cy (horiz) and x=cx (vert)
   double cx,cy;

   if (p1_inside && p2_inside)
   {
      cx = x1<x2 ? rx1 : rx2;
      cy = y1<y2 ? ry1 : ry2;
   }
   else if (p1_inside)
   {
      cx = x1<x2 ? rx2 : rx1;
      cy = y1<y2 ? ry2 : ry1;
   }
   else if (p2_inside)
   {
      cx = x1<x2 ? rx1 : rx2;
      cy = y1<y2 ? ry1 : ry2;
   }
   else
   {
      cx = x1<x2 ? rx2 : rx1;
      cy = y1<y2 ? ry2 : ry1;
   }

   // first, deal with the special cases: line is vert or horiz
   if (x1==x2)
   {
      if (x1<rx1) x1=rx1;
      if (x1>rx2) x1=rx2;
      if (x1!=rx1 && x1!=rx2) y1=cy;
      return;
   }

   if (y1==y2)
   {
      if (y1<ry1) y1=ry1;
      if (y1>ry2) y1=ry2;
      if (y1!=ry1 && y1!=ry2) x1=cx;
      return;
   }

   // write the line into y=ax+b form
   double a = (y2-y1)/(x2-x1);
   double b = y1 - a*x1;

   double xx,yy;

   // clip to the y=cy horizontal edge of the rect
   xx = (cy-b)/a;  //==> (xx,cy)

   // clip to the x=cx vertical edge of the rect
   yy = a*cx+b;  //==> (cx,yy)

   if (xx>=rx1 && xx<=rx2) {
         x1 = xx;
         y1 = cy;
   } else if (yy>=ry1 && yy<=ry2) {
         x1 = cx;
         y1 = yy;
   } else {
         x1 = cx;
         y1 = cy;
   }
   // default: rect and line don't have any common point :-(
   // leave line unchanged
}


int arrowcoords(Tcl_Interp *interp, int argc, const char **argv)
{
      if (argc!=18) {interp->result="17 args expected"; return TCL_ERROR;}

      // args 1..4: coords of source module rectangle
      double src_x1 = atof(argv[1]),
             src_y1 = atof(argv[2]),
             src_x2 = atof(argv[3]),
             src_y2 = atof(argv[4]);

      // args 5..8: coords of destination module rectangle
      double dest_x1 = atof(argv[5]),
             dest_y1 = atof(argv[6]),
             dest_x2 = atof(argv[7]),
             dest_y2 = atof(argv[8]);

      // args 9..12: gate indices and gate vector sizes
      int    src_i = atoi(argv[9]),
             src_n = atoi(argv[10]);
      int    dest_i = atoi(argv[11]),
             dest_n = atoi(argv[12]);

      // arg 13: mode: a(uto), m(anual), n, e, w, s (default is 'a')
      int    mode = argv[13][0]=='-' ? 'a' : argv[13][0];

      // args 14..17: destination anchoring (default value 50%)
      double src_anch_dx =  argv[14][0]=='-' ? 50 : atof(argv[14]),
             src_anch_dy =  argv[15][0]=='-' ? 50 : atof(argv[15]);
      double dest_anch_dx = argv[16][0]=='-' ? 50 : atof(argv[16]),
             dest_anch_dy = argv[17][0]=='-' ? 50 : atof(argv[17]);

      double src_x, src_y, dest_x, dest_y;

      // error checks
      if (!strchr("amnews",mode)) {
          interp->result="mode must be one of (a,m,n,e,w,s)";
          return TCL_ERROR;
      }

      // see if the two rects are the same, one is in the other etc.
      int same_rect = 0;
      int src_within_dest = 0;
      int dest_within_src = 0;

      if (src_x1==dest_x1 && src_x2==dest_x2 && src_y1==dest_y1 && src_y2==dest_y2)
         same_rect = 1;
      else if (src_x1<=dest_x1 && src_x2>=dest_x2 && src_y1<=dest_y1 && src_y2>=dest_y2)
         dest_within_src = 1;
      else if (src_x1>=dest_x1 && src_x2<=dest_x2 && src_y1>=dest_y1 && src_y2<=dest_y2)
         src_within_dest = 1;

      // some useful values...
      double src_width = src_x2-src_x1,
             src_height = src_y2-src_y1,
             dest_width = dest_x2-dest_x1,
             dest_height = dest_y2-dest_y1;

      // prepare for mode 'm'
      if (mode!='m') src_anch_dx=src_anch_dy=dest_anch_dx=dest_anch_dy=50;
      src_x = src_x1 + src_anch_dx*src_width/100;
      src_y = src_y1 + src_anch_dy*src_height/100;
      dest_x = dest_x1 + dest_anch_dx*dest_width/100;
      dest_y = dest_y1 + dest_anch_dy*dest_height/100;

      // do all rectangle relations one-by-one
      if (same_rect)
      {
         // a - height>width: E or W
         //     otherwise:    N or S
         if (mode=='a')
             mode = (src_height>src_width) ? 'e' : 'n';

         //  E,W - connection points E or W. Vert shift by gate indices
         //  N,S - connection points N or S. Horiz shift by gate indices
         switch (mode)
         {
           case 'n':
           case 's':
             src_x = dest_x = src_x1 + (src_i+1) * src_width / (src_n+1);
             src_y = src_y1; dest_y = src_y2;
             break;
           case 'e':
           case 'w':
             src_x = src_x1; dest_x = src_x2;
             src_y = dest_y = src_y1 + (src_i+1) * src_height / (src_n+1);
             break;
           case 'm':
             clip_line_to_rect(src_x,src_y,dest_x,dest_y, src_x1,src_y1,src_x2,src_y2);
             clip_line_to_rect(dest_x,dest_y,src_x,src_y, dest_x1,dest_y1,dest_x2,dest_y2);
             break;
         }
      }
      else if (src_within_dest)
      {
         // a - N,E,S,W (to nearest border,calculated from side)
         if (mode=='a')
         {
             double top =     src_y1 - dest_y1,
                    bottom =  dest_y2 - src_y2,
                    left =    src_x1 - dest_x1,
                    right =   dest_x2 - src_x2;
             if (top<=bottom && top<=left && top<=right)
                  mode = 'n';
             else if (right<=bottom && right<=left && right<=top)
                  mode = 'e';
             else if (left<=bottom && left<=right && left<=top)
                  mode = 'w';
             else if (bottom<=left && bottom<=right && bottom<=top)
                  mode = 's';
         }

         //  E,W - connection points E or W. Vert shift by gate indices
         //  N,S - connection points N or S. Horiz shift by gate indices
         switch (mode)
         {
           case 'n':
             src_x = dest_x = src_x1 + (src_i+1) * src_width / (src_n+1);
             src_y = src_y1; dest_y = dest_y1;
             break;
           case 's':
             src_x = dest_x = src_x1 + (src_i+1) * src_width / (src_n+1);
             src_y = src_y2; dest_y = dest_y2;
             break;
           case 'e':
             src_x = src_x2; dest_x = dest_x2;
             src_y = dest_y = src_y1 + (src_i+1) * src_height / (src_n+1);
             break;
           case 'w':
             src_x = src_x1; dest_x = dest_x1;
             src_y = dest_y = src_y1 + (src_i+1) * src_height / (src_n+1);
             break;
           case 'm':
             clip_line_to_rect(src_x,src_y,dest_x,dest_y, src_x1,src_y1,src_x2,src_y2);
             clip_line_to_rect(dest_x,dest_y,src_x,src_y, dest_x1,dest_y1,dest_x2,dest_y2);
             break;
         }
      }
      else if (dest_within_src)
      {
         // a - N,E,S,W (to nearest border,calculated from side)
         if (mode=='a')
         {
             double top =     dest_y1 - src_y1,
                    bottom =  src_y2 - dest_y2,
                    left =    dest_x1 - src_x1,
                    right =   src_x2 - dest_x2;
             if (top<=bottom && top<=left && top<=right)
                  mode = 'n';
             else if (right<=bottom && right<=left && right<=top)
                  mode = 'e';
             else if (left<=bottom && left<=right && left<=top)
                  mode = 'w';
             else if (bottom<=left && bottom<=right && bottom<=top)
                  mode = 's';
         }

         //  E,W - connection points E or W. Vert shift by gate indices
         //  N,S - connection points N or S. Horiz shift by gate indices
         switch (mode)
         {
           case 'n':
             src_x = dest_x = dest_x1 + (dest_i+1) * dest_width / (dest_n+1);
             src_y = src_y1; dest_y = dest_y1;
             break;
           case 's':
             src_x = dest_x = dest_x1 + (dest_i+1) * dest_width / (dest_n+1);
             src_y = src_y2; dest_y = dest_y2;
             break;
           case 'e':
             src_x = src_x2; dest_x = dest_x2;
             src_y = dest_y = dest_y1 + (dest_i+1) * dest_height / (dest_n+1);
             break;
           case 'w':
             src_x = src_x1; dest_x = dest_x1;
             src_y = dest_y = dest_y1 + (dest_i+1) * dest_height / (dest_n+1);
             break;
           case 'm':
             clip_line_to_rect(src_x,src_y,dest_x,dest_y, src_x1,src_y1,src_x2,src_y2);
             clip_line_to_rect(dest_x,dest_y,src_x,src_y, dest_x1,dest_y1,dest_x2,dest_y2);
             break;
         }
      }
      else // non-overlapping rectangles
      {
         //  a - E,W if one module's y range is within y range of other module
         //      N,S if one module's x range is within x range of other module
         //      otherwise M mode with (50%,50%) (50%,50%)

         if (mode!='m')
         {
             if (src_y1>=dest_y1 && src_y1<=dest_y2 && src_y2>=dest_y1 && src_y2<=dest_y2)
             {
                 src_y = dest_y = src_y1 + (src_i+1) * src_height / (src_n+1);

                 if (src_x2<=dest_x1) // src is left of dest
                 {
                     src_x = src_x2; dest_x = dest_x1;
                 }
                 else // src is right of dest
                 {
                     src_x = src_x1; dest_x = dest_x2;
                 }
             }
             else if (dest_y1>=src_y1 && dest_y1<=src_y2 && dest_y2>=src_y1 && dest_y2<=src_y2)
             {
                 dest_y = src_y = dest_y1 + (dest_i+1) * dest_height / (dest_n+1);

                 if (dest_x2<=src_x1) // dest is above of src
                 {
                     dest_x = dest_x2; src_x = src_x1;
                 }
                 else // dest is right of src
                 {
                     dest_x = dest_x1; src_x = src_x2;
                 }
             }
             else if (src_x1>=dest_x1 && src_x1<=dest_x2 && src_x2>=dest_x1 && src_x2<=dest_x2)
             {
                 src_x = dest_x = src_x1 + (src_i+1) * src_width / (src_n+1);

                 if (src_y2<=dest_y1) // src is above of dest
                 {
                     src_y = src_y2; dest_y = dest_y1;
                 }
                 else // src is right of dest
                 {
                     src_y = src_y1; dest_y = dest_y2;
                 }
             }
             else if (dest_x1>=src_x1 && dest_x1<=src_x2 && dest_x2>=src_x1 && dest_x2<=src_x2)
             {
                 dest_x = src_x = dest_x1 + (dest_i+1) * dest_width / (dest_n+1);

                 if (dest_y2<=src_y1) // dest is above of src
                 {
                     dest_y = dest_y2; src_y = src_y1;
                 }
                 else // dest is below of src
                 {
                     dest_y = dest_y1; src_y = src_y2;
                 }
             }
             else
                 mode = 'm';
         }

         if (mode=='m')
         {
             clip_line_to_rect(src_x,src_y,dest_x,dest_y, src_x1,src_y1,src_x2,src_y2);
             clip_line_to_rect(dest_x,dest_y,src_x,src_y, dest_x1,dest_y1,dest_x2,dest_y2);
         }
      }

      sprintf(interp->result, "%g %g %g %g",
                             src_x, src_y, dest_x, dest_y);
      return TCL_OK;
}

