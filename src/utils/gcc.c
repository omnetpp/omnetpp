#include <string.h>
#include <stdio.h>
#include <unistd.h>

char *newv[64];
char arg[128];

/*
 * --- gcc emulator for AIX ---
 *
 * This program emulates gcc for OMNeT++ makefiles under AIX if only xlC
 * is installed.
 * Just compile it and put in in your path.
 *
 * The problem with xlC is the it does not recognize the '-x c++' option
 * (==following file is C++ source) and all C++ sources must have
 * the .C suffix.
 *
 * This program transforms the command line arguments, creates
 * soft links (foo.cc --> foo.C), and calls xlC.
 *					--VA
 */

int main(int argc, char *argv[])
{
    int i, n, len;

    for (i=1, n=1; i<argc; i++)
    {
	if (strcmp(argv[i],"-x")==0 || strcmp(argv[i],"c++")==0 )
	    continue;

	newv[n] = strdup( argv[i] );

	len = strlen( newv[n] );
	if (len>3 && strcmp( newv[n]+len-3, ".cc")==0)
	{
	   *(newv[n]+len-2) = 'C';
	   *(newv[n]+len-1) = '\0';
	   printf("--> ln -s %s %s\n", argv[i], newv[n] );
	   if (symlink( argv[i], newv[n] )!=0)
	   {
		fprintf(stderr, "Warning: could not create soft link (already exists?)\n");
	   }
	}
	n++;
    }
    newv[n] = NULL;

    printf("--> xlC");
    for (i=1;newv[i]!=NULL;i++)
	printf(" %s", newv[i]);
    printf("\n");

    execvp( "xlC", newv );

    fprintf(stderr, "Cannot exec xlC!\n");
    return 1;
}


