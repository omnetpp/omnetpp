/*
 * tkImgPNGInit.c --
 *
 *		This file initializes a package implementing a PNG photo image
 *      type for Tcl/Tk.  See the file tkImgPNG.c for the actual
 *      implementation.
 *
 * Copyright (c) 2005 Michael Kirkham <mikek@muonics.com> & Muonics
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tkImgPNGInit.c,v 1.3 2005/03/26 22:20:56 mikek Exp $
 */

#include <tcl.h>
#include <tk.h>
#if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION <6

extern Tk_PhotoImageFormat tkImgFmtPNG;

/*
 *----------------------------------------------------------------------
 *
 * Tkpng_Init --
 *
 *		Initialize the Tcl PNG package.
 *
 * Results:
 *		A standard Tcl result
 *
 * Side effects:
 *		PNG support added to the "photo" image type.
 *
 *----------------------------------------------------------------------
 */

int
Tkpng_Init(Tcl_Interp *interp)
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) {
		return TCL_ERROR;
	}
	if (Tcl_PkgRequire(interp, "Tcl", "8.4", 0) == NULL) {
		return TCL_ERROR;
	}
	if (Tk_InitStubs(interp, "8.4", 0) == NULL) {
		return TCL_ERROR;
	}
	if (Tcl_PkgRequire(interp, "Tk", "8.4", 0) == NULL) {
		return TCL_ERROR;
	}

	Tk_CreatePhotoImageFormat(&tkImgFmtPNG);

	if (Tcl_PkgProvide(interp, "tkpng", "0.7") != TCL_OK) {
		return TCL_ERROR;
	}

	return TCL_OK;
}

#endif