package org.omnetpp.scave.pychart;

import java.io.IOException;

import net.razorvine.pickle.PickleException;

public interface IScaveResultsPickleProvider {
    byte[] getScalarsPickle(String filter) throws PickleException, IOException;
    byte[] getVectorsPickle(String filter) throws PickleException, IOException;
    byte[] getStatisticsPickle(String filter) throws PickleException, IOException;
    byte[] getHistogramsPickle(String filter) throws PickleException, IOException;
}
