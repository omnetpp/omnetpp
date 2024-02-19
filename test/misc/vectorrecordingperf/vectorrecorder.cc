#include "vectorrecorder.h"

Define_Module(VectorRecorder);


void VectorRecorder::initialize()
{
    cEnvir *ev = getEnvir();
    std::vector<void*> vectorHandles;

    int numVectors = par("numVectors");
    for (int i = 0; i < numVectors; i++) {
        char vectorName[32];
        snprintf(vectorName, sizeof(vectorName), "dummy-vector-%d", i);
        vectorHandles.push_back(ev->registerOutputVector("dummy-module", vectorName));
    }

    int numValues = par("numValues");;
    simtime_t t = 0;
    simtime_t dt = 0.001;
    double vectorIndexMean = numVectors / 5.0;
    double geomP = 1 / vectorIndexMean;
    for (int i = 0; i < numValues; i++) {
        int vectorIndex = geometric(geomP);
        if (vectorIndex >= numVectors)
            vectorIndex = intuniform(0, numVectors-1);
        double value = dblrand();
        ev->recordInOutputVector(vectorHandles[vectorIndex], t, value);
        t += dt;
    }
}
