//-------------------------------------------------------------
// file: dist.cc
//-------------------------------------------------------------

#include <omnetpp.h>

class Dist : public cSimpleModule
{
    public:
      Module_Class_Members(Dist,cSimpleModule,16384)
      virtual void activity();
};

Define_Module( Dist );

void Dist::activity()
{
    cPar& variate = par("variate");
    long n = par("n");
    int numcells = par("numcells");
    long firstvals = par("firstvals");
    const char *excel = par("excel");
    const char *filename = par("file");

    char distname[50];
    variate.getAsText(distname,50);
    ev << "running:" << distname << endl;

    cDoubleHistogram h;
    h.setNumCells(numcells);
    h.setRangeAuto(firstvals,1);

    for (long i=0; i<n; i++)
    {
        double d = variate.doubleValue();
        h.collect(d);
    }

    ev << "done, writing file" << endl;

    FILE *f = fopen(filename, "w");
    fprintf(f,"\"x\",\"theoretical %s pdf\",\"measured %s\" pdf\n",distname, distname);

    for (int k=0; k<h.cells(); k++)
    {
        fprintf(f,"%lg,\"=",(h.basepoint(k)+h.basepoint(k+1))/2);
        fprintf(f,excel,k+2);
        fprintf(f,"\",%lg\n",h.cellPDF(k));
    }
    fclose(f);
}

