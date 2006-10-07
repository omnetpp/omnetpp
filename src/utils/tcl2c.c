//=========================================================================
//
//  TCL2C.C - part of
//                  OMNeT++/OMNEST
//        Discrete System Simulation in C++
//
//   Utility for embedding TCL code into C/C++ as large string constant.
//   Output is somewhat scrambled so that the TCL code is not readable
//   if someone looks into the resulting executables.
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>

#if defined(_WIN32) && !defined(__CYGWIN32__)

/*
 * On Windows, wildcards must be expanded by hand :-(((
 */
#define MUST_EXPAND_WILDCARDS 1
const char *findFirstFile(const char *mask);
const char *findNextFile();

/*
 * code for Windows/MSVC
 */
#include <io.h>
long handle;
struct _finddata_t fdata;
const char *findFirstFile(const char *mask)
{
    handle = _findfirst(mask, &fdata);
    if (handle<0) {_findclose(handle); return NULL;}
    return fdata.name;
}
const char *findNextFile()
{
    int done=_findnext(handle, &fdata);
    if (done) {_findclose(handle); return NULL;}
    return fdata.name;
}

/*

 *
 * code for Borland C -- currently not used
 *
#include <dir.h>
struct ffblk ffblk;
const char *findFirstFile(const char *mask)
{
    int done = findfirst(argv[k],&ffblk,0);
    return done ? NULL : ffblk.ff_name;
}
const char *findNextFile()
{
    int done = findnext(&ffblk);
    return done ? NULL : ffblk.ff_name;
}

*/

#endif /* _WIN32 */

int main(int argc, char **argv)
{
    int k;
    int c;
    int n;
    FILE *fin, *fout;

    int sum;

    if (argc<3) {
        fprintf(stderr,"usage: %s output-c-file input-tcl-files\n",argv[0]);
        return 1;
    }

    fout = fopen(argv[1],"w");
    if (!fout) {
        fprintf(stderr,"%s: cannot open %s for write\n",argv[0],argv[1]);
        return 1;
    }

    fprintf(fout,"static unsigned char tcl_code[] = {\n  ");

    sum = 0;
    for (k=2; k<argc; k++) {

        const char *fname;

#ifdef MUST_EXPAND_WILDCARDS
        fname = findFirstFile(argv[k]);
        if (!fname) {
            fprintf(stderr,"%s: not found: %s\n",argv[0],argv[k]);
            return 1;
        }

        while (fname)
        {
#else
           fname = argv[k];
#endif
           fin = fopen(fname,"r");
           if (!fin) {
               fprintf(stderr,"%s: cannot open %s for read\n",argv[0],fname);
               return 1;
           }

           fprintf(fout,"/* %s */\n  ",fname);

           n = 0;
           while ((c=fgetc(fin))!=EOF) {
                c = 255-c;
                /* c = (c+sum)%256;
                   sum = (sum+c)%219;
                */
                fprintf(fout,"%d,",c);
                if (++n == 20) {
                    n = 0;
                    fprintf(fout,"\n  ");
                }
           }

           fclose(fin);
           if (n>0)  fprintf(fout,"\n  ");

#ifdef MUST_EXPAND_WILDCARDS
           fname = findNextFile();
        }
#endif
    }
    fprintf(fout,"0\n};\n\n");

    fprintf(fout,
         "/* decoding part */\n"
         "{\n"
         "  int i;\n"
         "  unsigned char *s = tcl_code;\n"
         "  for (i=0; i<sizeof(tcl_code)-1; i++,s++) {\n"
         "    *s = 255-(*s);\n"
         "  }\n"
         "}\n"
         /* "{\n"
            "  int i,sum=0;\n"
            "  unsigned char *s = tcl_code;\n"
            "  for (i=0; i<sizeof(tcl_code)-1; i++,s++) {\n"
            "    int c=*s;\n"
            "    *s = (*s-sum+256)%256;\n"
            "    sum=(sum+c)%219;\n"
            "  }\n"
            "}\n"
         */
    );

    fclose(fout);
    return 0;
}

