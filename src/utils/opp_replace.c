/*
 * Title: Mini (sed like) replacement application
 * Author: Balazs KELEMEN (kelemen@sch.bme.hu)
 * Date: Mar 14 2000
 *
 * Description: Replaces <pattern> with <value> in <proto-file> file
 *     and write out result to <ouput-file> file.
 *     Using - (minus) in place of filenames means stdin and stdout.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BUF_L         128
#define CACHE_L (BUF_L-1)

#define OFFSET(i)       (i % BUF_L)

static int _readOffs;
static int _writeOffs;
char _buffer[BUF_L];


void writeValue(FILE *fp, char *val);
void emptyCache(FILE *fp, int toOffs);

int main(int argc, char *argv[])
{
    char *protoFn, *outputFn, *pattern, *val;
    FILE *protoFp, *outputFp;
    int patternOffs;
    int patternL;
    int isWriting;

    if(argc < 5)
    {
        fprintf(stderr, "Usage:\n\t%s <proto-file> <output-file> <pattern> <value>\n", argv[0]);
        return 1;
    }

    protoFn = argv[1];
    outputFn = argv[2];
    pattern = argv[3];
    patternL = strlen(pattern);
    val = argv[4];

    if(patternL > CACHE_L)
    {
        fprintf(stderr, "Sorry! Maximum allowed pattern length is %d\n",
                CACHE_L);
        return 1;
    }

    /* open prototype input */
    if(strcmp(protoFn, "-") == 0)
    {
        protoFp = stdin;
    }
    else if((protoFp = fopen(protoFn, "rt")) == NULL)
    {
        fprintf(stderr, "Can't open prototype file: '%s': %s\n", protoFn,
                strerror(errno) );
        return 1;
    }

    /* open output */
    if(strcmp(outputFn, "-") == 0)
    {
        outputFp = stdout;
    }
    else if((outputFp = fopen(outputFn, "wt")) == NULL)
    {
        fprintf(stderr, "Can't open output file: '%s': %s\n", outputFn,
                strerror(errno) );
        return 1;
    }

    _readOffs = 0;
    _writeOffs = 0;
    patternOffs = 0;
    isWriting = 0;
    while((_buffer[OFFSET(_readOffs)] = fgetc(protoFp)) != EOF)
    {
        /* check match */
        if(_buffer[OFFSET(_readOffs)] == pattern[patternOffs])
        {
            patternOffs++;
            if(patternOffs == patternL)
            {
                emptyCache(outputFp, _readOffs-patternL+1);
                isWriting = 0;
                writeValue(outputFp, val);
                patternOffs = 0;
                _writeOffs = _readOffs;
                continue;
            }
        }
        else
        {
            patternOffs = 0;
        }

        if((_readOffs - _writeOffs) >= CACHE_L)
            isWriting = 1;

        if(isWriting)
        {
            fputc(_buffer[OFFSET(_writeOffs)], outputFp);
            _writeOffs++;
        }

        _readOffs++;
    } /* while */

    emptyCache(outputFp, _readOffs);

    fclose(outputFp);
    fclose(protoFp);

    return 0;
} /* main */


void writeValue(FILE *fp, char *val)
{
    fwrite(val, 1, strlen(val), fp);
    return;
}

void emptyCache(FILE *fp, int toOffs)
{
    while(_writeOffs != toOffs)
    {
        fputc(_buffer[OFFSET(_writeOffs)], fp);
        _writeOffs++;
    }

    return;
}

