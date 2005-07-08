#include <assert.h>
#include <deque>
#include <vector>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>


class RandomSourceNode : public SourceNode
{
    protected:
        int n;  // total to be generated
        int k;  // already generated
    public:
        RandomSourceNode(int count) {n=count; k=0;}
        virtual ~RandomSourceNode() {}
        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;
};

bool RandomSourceNode::isReady() const
{
    return !finished();
}

void RandomSourceNode::process()
{
    Datum a;
    a.x = k;
    a.y = rand();
    DBG(("rand: produced (%lg,%lg)\n", a.x, a.y));
    out()->write(&a,1);
    k++;
}

bool RandomSourceNode::finished() const
{
    return k==n;
}

//---------------------------------

class PrinterNode : public SingleSinkNode
{
    public:
        PrinterNode() {}
        virtual ~PrinterNode() {}
        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;
};

bool PrinterNode::isReady() const
{
    return in()->length()>0;
}

void PrinterNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        in()->read(&a,1);
        printf("--> (%lg, %lg)\n", a.x, a.y);
    }
}

bool PrinterNode::finished() const
{
    return in()->eof();
}

main()
{
    clock_t t0 = clock();
    try
    {
        DataflowManager net;

        /*
        RandomSourceNode *src = new RandomSourceNode(17);
        SlidingWindowAverageNode *winavg = new SlidingWindowAverageNode(5);
        PrinterNode *sink = new PrinterNode();

        net.addNode(src);
        net.addNode(winavg);
        net.addNode(sink);

        net.connect(&(src->out), &(winavg->in), winavg);
        net.connect(&(winavg->out), &(sink->in), sink);
        */

        //VectorFileReaderNode *src = new VectorFileReaderNode("demo.vec", 200);
        VectorFileReaderNode *src = new VectorFileReaderNode("big.vec", 64*1024);
        PrinterNode *pr1 = new PrinterNode();
        PrinterNode *pr2 = new PrinterNode();

        net.addNode(src);
        net.addNode(pr1);
        net.addNode(pr2);

        net.connect(src->addVector(7), &(pr1->in), pr1);
        net.connect(src->addVector(12), &(pr2->in), pr2);

        net.execute();
    }
    catch (Exception *ex)
    {
        printf("exception: %s", ex->message());
        delete ex;
    }

    clock_t t1 = clock();
    fprintf(stderr,"time: %lg\n", (t1-t0)/(double)1000);

    return 0;
}


