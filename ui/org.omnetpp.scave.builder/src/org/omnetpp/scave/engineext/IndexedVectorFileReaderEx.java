package org.omnetpp.scave.engineext;

import org.omnetpp.common.engine.BigDecimal;
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
		return copy(super.getEntryBySerial((int)serial));
	}

	@Override
	public OutputVectorEntry getEntryByEventnum(int eventNum, boolean after) {
		return copy(super.getEntryByEventnum(eventNum, after));
	}

	@Override
	public OutputVectorEntry getEntryBySimtime(BigDecimal simtime, boolean after) {
		return copy(super.getEntryBySimtime(simtime, after));
	}
	
	private static OutputVectorEntry copy(OutputVectorEntry entry) {
		if (entry == null)
			return null;
		OutputVectorEntry ownedEntry = new OutputVectorEntry(
											entry.getSerial(),
											entry.getEventNumber(),
											new BigDecimal(entry.getSimtime()), // because entry.getSimtime() points to the class member 
											entry.getValue());
		return ownedEntry;
	}
}
