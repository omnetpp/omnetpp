//==========================================================================
//  TKCMD.CC -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sstream>

#include "tklib.h"
#include "arrow.h"
#include "parsened.h"


#define min(a,b)  ((a)<(b) ? (a) : (b))

//----------------------------------------------------------------
int arrowcoords_cmd(ClientData, Tcl_Interp *, int, const char **);
int parsened_cmd(ClientData, Tcl_Interp *, int, const char **);
int hsbToRgb_cmd(ClientData, Tcl_Interp *, int, const char **);
int colorizeImage_cmd(ClientData, Tcl_Interp *, int, const char **);

// command table
OmnetTclCommand tcl_commands[] = {
   // misc
   { "opp_arrowcoords",   arrowcoords_cmd  }, // args: 20 args; see arrow.cc
   { "opp_parsened",      parsened_cmd     }, // args: NED file name
   { "opp_hsb_to_rgb",    hsbToRgb_cmd     }, // args: <@hhssbb> ret: <#rrggbb>
   { "opp_colorizeimage", colorizeImage_cmd}, // args: <image> ... ret: -
   // end of list
   { NULL, },
};

int arrowcoords_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    return arrowcoords(interp,argc,argv);
}

int parsened_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    int nedfilekey = -1;
    const char *nedarray = NULL;
    const char *errorsarray = NULL;
    const char *filename = NULL;
    const char *nedtext = NULL;

    for (int i=1; i<argc; i++)
    {
       if (i+1>=argc)
       {
           interp->result="wrong arg count -- missing argument?";
           return TCL_ERROR;
       }

       if (strcmp(argv[i],"-file")==0)
          filename = argv[++i];
       else if (strcmp(argv[i],"-text")==0)
          nedtext = argv[++i];
       else if (strcmp(argv[i],"-nedarray")==0)
          nedarray = argv[++i];
       else if (strcmp(argv[i],"-errorsarray")==0)
          errorsarray = argv[++i];
       else if (strcmp(argv[i],"-nedfilekey")==0)
          nedfilekey = atoi(argv[++i]);
       else
       {
           sprintf(interp->result, "unrecognized arg %s", argv[i]);
           return TCL_ERROR;
       }
    }

    if ((filename && nedtext) || (!filename && !nedtext))
    {
       interp->result="one of -file and -text options must be present";
       return TCL_ERROR;
    }
    if (nedfilekey==-1 || !nedarray || !errorsarray)
    {
       interp->result="-nedfilekey, -nedarray, -errorsarray options must be present";
       return TCL_ERROR;
    }

    if (nedtext)
        return parseNedText(interp, nedtext,  nedfilekey, nedarray, errorsarray);
    else
        return parseNedFile(interp, filename, nedfilekey, nedarray, errorsarray);
}

//
// HSB-to-RGB conversion
// source: http://nuttybar.drama.uga.edu/pipermail/dirgames-l/2001-March/006061.html
// Input:   hue, saturation, and brightness as floats scaled from 0.0 to 1.0
// Output:  red, green, and blue as floats scaled from 0.0 to 1.0
//
static void hsbToRgb(double hue, double saturation, double brightness,
                     double& red, double& green, double &blue)
{
   if (brightness == 0.0)
   {   // safety short circuit again
       red   = 0.0;
       green = 0.0;
       blue  = 0.0;
       return;
   }

   if (saturation == 0.0)
   {   // grey
       red   = brightness;
       green = brightness;
       blue  = brightness;
       return;
   }

   float domainOffset;         // hue mod 1/6
   if (hue < 1.0/6)
   {   // red domain; green ascends
       domainOffset = hue;
       red   = brightness;
       blue  = brightness * (1.0 - saturation);
       green = blue + (brightness - blue) * domainOffset * 6;
   }
     else if (hue < 2.0/6)
     { // yellow domain; red descends
       domainOffset = hue - 1.0/6;
       green = brightness;
       blue  = brightness * (1.0 - saturation);
       red   = green - (brightness - blue) * domainOffset * 6;
     }
     else if (hue < 3.0/6)
     { // green domain; blue ascends
       domainOffset = hue - 2.0/6;
       green = brightness;
       red   = brightness * (1.0 - saturation);
       blue  = red + (brightness - red) * domainOffset * 6;
     }
     else if (hue < 4.0/6)
     { // cyan domain; green descends
       domainOffset = hue - 3.0/6;
       blue  = brightness;
       red   = brightness * (1.0 - saturation);
       green = blue - (brightness - red) * domainOffset * 6;
     }
     else if (hue < 5.0/6)
     { // blue domain; red ascends
       domainOffset = hue - 4.0/6;
       blue  = brightness;
       green = brightness * (1.0 - saturation);
       red   = green + (brightness - green) * domainOffset * 6;
     }
     else
     { // magenta domain; blue descends
       domainOffset = hue - 5.0/6;
       red   = brightness;
       green = brightness * (1.0 - saturation);
       blue  = red - (brightness - green) * domainOffset * 6;
     }
}

inline int h2d(char c)
{
    if (c>='0' && c<='9') return c-'0';
    if (c>='A' && c<='F') return c-'A'+10;
    if (c>='a' && c<='f') return c-'a'+10;
    return 0;
}

int hsbToRgb_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   const char *hsb = argv[1];
   if (hsb[0]!='@' || strlen(hsb)!=7) {Tcl_SetResult(interp, "malformed HSB color, format is @hhssbb where h,s,b are hex digits", TCL_STATIC); return TCL_ERROR;}

   // parse hsb
   double hue =        (h2d(hsb[1])*16+h2d(hsb[2]))/256.0;
   double saturation = (h2d(hsb[3])*16+h2d(hsb[4]))/256.0;
   double brightness = (h2d(hsb[5])*16+h2d(hsb[6]))/256.0;

   // convert
   double red, green, blue;
   hsbToRgb(hue, saturation, brightness, red, green, blue);

   // produce rgb
   char rgb[10];
   sprintf(rgb,"#%2.2x%2.2x%2.2x",
                int(min(red*256,255)),
                int(min(green*256,255)),
                int(min(blue*256,255))
          );
   Tcl_SetResult(interp, rgb, TCL_VOLATILE);
   return TCL_OK;
}

int colorizeImage_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=4) {Tcl_SetResult(interp, "3 args expected", TCL_STATIC); return TCL_ERROR;}
   const char *imgname = argv[1];
   const char *targetcolorname = argv[2];
   const char *weightstr = argv[3]; // 0-100

   Tk_PhotoHandle imghandle = Tk_FindPhoto(interp, TCLCONST(imgname));
   if (!imghandle)
   {
       Tcl_SetResult(interp, "image doesn't exist or is not a photo image", TCL_STATIC);
       return TCL_ERROR;
   }
   Tk_PhotoImageBlock imgblock;
   Tk_PhotoGetImage(imghandle, &imgblock);

   if (imgblock.pixelSize!=4)
   {
       Tcl_SetResult(interp, "unsupported pixelsize in photo image", TCL_STATIC);
       return TCL_ERROR;
   }

   XColor *targetcolor = Tk_GetColor(interp, Tk_MainWindow(interp), TCLCONST(targetcolorname));
   if (!targetcolor)
   {
       Tcl_SetResult(interp, "invalid color", TCL_STATIC);
       return TCL_ERROR;
   }
   int rdest = targetcolor->red / 256;  // scale down to 8 bits
   int gdest = targetcolor->green / 256;
   int bdest = targetcolor->blue / 256;
   Tk_FreeColor(targetcolor);

   double weight = atol(weightstr)/100.0;
   if (weight<0 || weight>1.0)
   {
       Tcl_SetResult(interp, "colorizing weight is out of range, should be between 0 and 100", TCL_STATIC);
       return TCL_ERROR;
   }

   int redoffset = imgblock.offset[0];
   int greenoffset = imgblock.offset[1];
   int blueoffset = imgblock.offset[2];
   for (int y=0; y<imgblock.height; y++)
   {
       unsigned char *pixel = imgblock.pixelPtr + y*imgblock.pitch;
       for (int x=0; x<imgblock.width; x++, pixel+=imgblock.pixelSize)
       {
           // extract
           int r = pixel[redoffset];
           int g = pixel[greenoffset];
           int b = pixel[blueoffset];

           // transform
           int lum = (int)(0.2126*r + 0.7152*g + 0.0722*b);
           r = (int)((1-weight)*r + weight*lum*rdest/128.0);
           g = (int)((1-weight)*g + weight*lum*gdest/128.0);
           b = (int)((1-weight)*b + weight*lum*bdest/128.0);

           // fix range
           r = r<0 ? 0 : r>255 ? 255 : r;
           g = g<0 ? 0 : g>255 ? 255 : g;
           b = b<0 ? 0 : b>255 ? 255 : b;

           // and put back
           pixel[redoffset] = r;
           pixel[greenoffset] = g;
           pixel[blueoffset] = b;
       }
   }
   return TCL_OK;
}

