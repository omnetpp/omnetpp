package org.omnetpp.scave.engineext;

import org.omnetpp.scave.engine.IndexedVectorFileReader;
import org.omnetpp.scave.engine.OutputVectorEntry;

/**
 * Reader for an indexed vector file.
 * It copies the structures returned by an IndexedVectorFileReader.getEntryBySerial(),
 * because the underlying C++ objects of OutputVectorEntries can be
 * deleted by the next getEntryBySerial() call, so their use is 
 * dangerous.
 *
 * @author tomi
 */
public class IndexedVectorFileReaderEx extends IndexedVectorFileReader {

	public IndexedVectorFileReaderEx(String filename, int vectorId) {
		super(filename, vectorId);
	}
	
	public OutputVectorEntry getEntryBySerial(int serial) {
		OutputVectorEntry entry = super.getEntryBySerial((int)serial);
		return new OutputVectorEntry(entry.getSerial(), entry.getSimtime(), entry.getValue());
	}
}
