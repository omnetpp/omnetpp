package org.omnetpp.scave.editors;

import java.io.IOException;

import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.pychart.IScaveResultsPickleProvider;
import org.omnetpp.scave.python.ResultPicklingUtils;

import net.razorvine.pickle.PickleException;

class ResultsProvider implements IScaveResultsPickleProvider {
    ResultFileManager rfm;

    public ResultsProvider(ResultFileManager rfm) {
        this.rfm = rfm;
    }

    @Override
    public byte[] getScalarsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.getScalarsPickle(rfm, filter);
    }

    @Override
    public byte[] getVectorsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.getVectorsPickle(rfm, filter);
    }

    @Override
    public byte[] getStatisticsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.getStatisticsPickle(rfm, filter);
    }

    @Override
    public byte[] getHistogramsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.getHistogramsPickle(rfm, filter);
    }
}