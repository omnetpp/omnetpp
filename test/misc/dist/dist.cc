//-------------------------------------------------------------
// File: dist.cc
// Purpose: testing the distribution functions in OMNeT++
//-------------------------------------------------------------

#include <algorithm>
#include <string>
#include <ctype.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Dist : public cSimpleModule
{
    public:
      Dist() : cSimpleModule(16384) {}
      virtual void activity();
};

Define_Module(Dist);

void Dist::activity()
{
    cPar& variate = par("variate");
    long n = par("n");
    bool discrete = par("discrete");
    int numcells = par("numcells");
    long firstvals = par("firstvals");
    const char *excel = par("excel");
    const char *filename = par("file");

    std::string excel_str = excel;
    std::replace(excel_str.begin(), excel_str.end(), ',', ';');
    excel = excel_str.c_str();

    std::string distname = variate.str();
    EV << "running: " << distname << endl;

    // generate histogram
    cHistogramBase *h;
    if (discrete)
        h = new cLongHistogram;
    else
        h = new cDoubleHistogram;
    h->setNumCells(numcells);
    h->setRangeAuto(firstvals, 1);

    for (long i = 0; i < n; i++) {
        double d = variate.doubleValue();
        h->collect(d);
    }

    // automatic filename
    char buf[500];
    if (filename[0] == '\0') {
        strcpy(buf, distname.c_str());
        for (char *s = buf; *s; s++)
            if (!isalnum(*s) && *s != '(' && *s != ')' && *s != ',' && *s != '-' && *s != '+')
                *s = '_';

        strcat(buf, ".csv");
        filename = buf;
    }

    EV << "writing file: " << filename << endl;

    // write file
    FILE *f = fopen(filename, "w");
    fprintf(f, "\"x\",\"theoretical %s pdf\",\"measured %s pdf\",measured mean,measured std dev\n", distname.c_str(), distname.c_str());
    fprintf(f, ",,,%lg,%lg\n", h->getMean(), h->getStddev());
    const int off = 3;  // data begin on line 3 in the Excel sheet
    for (int k = 0; k < h->getNumCells(); k++) {
        fprintf(f, "%lg,\"=", (h->getBasepoint(k)+h->getBasepoint(k+1))/2);
        fprintf(f, excel, k+off, k+off, k+off, k+off, k+off, k+off, k+off, k+off, k+off, k+off);
        fprintf(f, "\",%lg\n", h->getCellPDF(k));
    }
    fclose(f);

    // next line is for Tkenv, so that one can inspect the histogram
    wait(1.0);

    delete h;
}

