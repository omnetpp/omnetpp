package org.omnetpp.scave.editors.datatable;

import org.eclipse.jface.viewers.Viewer;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.virtualtable.IVirtualTableContentProvider;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.OutputVectorEntry;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engineext.IndexedVectorFileReaderEx;
import org.omnetpp.scave.model2.ComputedResultFileLocator;

/**
 * Implementation of the IVirtualTableContentProvider interface for 
 * the table of vector data.
 *
 * @author tomi
 */
public class VectorResultContentProvider implements IVirtualTableContentProvider<OutputVectorEntry> {

	private IndexedVectorFileReaderEx reader;
	
	public OutputVectorEntry getApproximateElementAt(double percentage) {
		if (reader != null)
			return reader.getEntryBySerial((int)(percentage * reader.getNumberOfEntries()));
		else
			return null;
	}

	public long getApproximateNumberOfElements() {
		if (reader != null)
			return reader.getNumberOfEntries();
		else
			return 0;
	}

	public double getApproximatePercentageForElement(OutputVectorEntry element) {
		if (reader != null)
			return ((double)element.getSerial()) / reader.getNumberOfEntries();
		else
			return 0.0;
	}

	public long getDistanceToElement(OutputVectorEntry sourceElement, OutputVectorEntry targetElement, long limit) {
		if (reader != null) {
			long delta = targetElement.getSerial() - sourceElement.getSerial();

			if (Math.abs(delta) > Math.abs(limit) || Math.signum(delta) != Math.signum(limit))
				return limit;
			else
				return delta;
		}
		else 
			return 0;
	}

	public long getDistanceToFirstElement(OutputVectorEntry element, long limit) {
		if (reader != null)
			return Math.min(element.getSerial(), limit);
		else
			return 0;
	}

	public long getDistanceToLastElement(OutputVectorEntry element, long limit) {
		if (reader != null) {
			int lastSerial = reader.getNumberOfEntries() - 1;
			return Math.min(lastSerial - element.getSerial(), limit);
		}
		else
			return 0;
	}

	public OutputVectorEntry getFirstElement() {
		if (reader != null)
			return reader.getEntryBySerial(0);
		else
			return null;
	}

	public OutputVectorEntry getLastElement() {
		if (reader != null)
			return reader.getEntryBySerial(reader.getNumberOfEntries() - 1);
		else
			return null;
	}

	public OutputVectorEntry getNeighbourElement(OutputVectorEntry element, long distance) {
		if (reader != null)
			return reader.getEntryBySerial((int)(element.getSerial() + distance));
		else
			return null;
	}

	public OutputVectorEntry getClosestElement(OutputVectorEntry element) {
		return element;
	}
	
	public OutputVectorEntry getElementBySerial(int serial) {
		return reader != null ? reader.getEntryBySerial(serial) : null;
	}
	
	public OutputVectorEntry getElementBySimulationTime(BigDecimal time, boolean after) {
		return reader != null ? reader.getEntryBySimtime(time, after) : null;
	}
	
	public OutputVectorEntry getElementByEventNumber(int eventNumber, boolean after) {
		return reader != null ? reader.getEntryByEventnum(eventNumber, after) : null;
	}
	
	public void dispose() {
		if (reader != null) {
			reader.delete();
			reader = null;
		}
	}

	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
		if (reader != null) {
			reader.delete();
			reader = null;
		}
		
		if (newInput instanceof VectorResult) {
			VectorResult vector = (VectorResult)newInput;
			int vectorId = vector.getVectorId();
			String filename = vector.getFileRun().getFile().getFileSystemFilePath();
			if (vector.getComputed()) // XXX filename should be ok
				filename = ComputedResultFileLocator.instance().getFileNameFor(vector);
				
			try {
				reader = new IndexedVectorFileReaderEx(filename, vectorId);
			}
			catch (Exception e) {
				ScavePlugin.logError("Cannot open index file: "+filename, e);
			}
		}
	}
}
