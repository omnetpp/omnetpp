/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.Viewer;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.virtualtable.IVirtualTableContentProvider;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.IndexedVectorFileReader;
import org.omnetpp.scave.engine.IndexedVectorFileReader.Adapter;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.VectorDatum;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.model2.ResultItemRef;

/**
 * Implementation of the IVirtualTableContentProvider interface for
 * the table of vector data.
 *
 * @author tomi
 */
public class VectorResultContentProvider implements IVirtualTableContentProvider<VectorDatum> {

    private IndexedVectorFileReader reader;
    int vectorId;

    public VectorDatum getApproximateElementAt(double percentage) {
        if (reader != null)
            return reader.getEntryBySerial(vectorId, (int)(percentage * reader.getNumberOfEntries(vectorId)));
        else
            return null;
    }

    public long getApproximateNumberOfElements() {
        if (reader != null)
            return reader.getNumberOfEntries(vectorId);
        else
            return 0;
    }

    public double getApproximatePercentageForElement(VectorDatum element) {
        if (reader != null)
            return ((double)element.getSerial()) / reader.getNumberOfEntries(vectorId);
        else
            return 0.0;
    }

    public int compare(VectorDatum element1, VectorDatum element2) {
        return element1.getSerial() - element2.getSerial();
    }

    public long getDistanceToElement(VectorDatum sourceElement, VectorDatum targetElement, long limit) {
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

    public long getDistanceToFirstElement(VectorDatum element, long limit) {
        if (reader != null)
            return Math.min(element.getSerial(), limit);
        else
            return 0;
    }

    public long getDistanceToLastElement(VectorDatum element, long limit) {
        if (reader != null) {
            int lastSerial = reader.getNumberOfEntries(vectorId) - 1;
            return Math.min(lastSerial - element.getSerial(), limit);
        }
        else
            return 0;
    }

    public VectorDatum getFirstElement() {
        if (reader != null)
            return reader.getEntryBySerial(vectorId, 0);
        else
            return null;
    }

    public VectorDatum getLastElement() {
        if (reader != null)
            return reader.getEntryBySerial(vectorId, reader.getNumberOfEntries(vectorId) - 1);
        else
            return null;
    }

    public VectorDatum getNeighbourElement(VectorDatum element, long distance) {
        if (reader != null)
            return reader.getEntryBySerial(vectorId, (int)(element.getSerial() + distance));
        else
            return null;
    }

    public VectorDatum getClosestElement(VectorDatum element) {
        return element;
    }

    public VectorDatum getElementBySerial(int serial) {
        return reader != null ? reader.getEntryBySerial(vectorId, serial) : null;
    }

    public VectorDatum getElementBySimulationTime(BigDecimal time, boolean after) {
        return reader != null ? reader.getEntryBySimtime(vectorId, time, after) : null;
    }

    public VectorDatum getElementByEventNumber(long eventNumber, boolean after) {
        return reader != null ? reader.getEntryByEventnum(vectorId, eventNumber, after) : null;
    }

    public void dispose() {
        if (reader != null) {
            reader.delete();
            reader = null;
        }
    }

    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
        Assert.isTrue(oldInput == null || oldInput instanceof ResultItemRef);
        Assert.isTrue(newInput == null || newInput instanceof ResultItemRef);

        if (reader != null) {
            reader.delete();
            reader = null;
        }

        if (newInput instanceof ResultItemRef) {
            ResultItem item = ((ResultItemRef)newInput).resolve();
            if (item instanceof VectorResult) {
                VectorResult vector = (VectorResult)item;
                vectorId = vector.getVectorId();
                String filename = vector.getFileRun().getFile().getFileSystemFilePath();

                try {
                    reader = new IndexedVectorFileReader(filename, true, (Adapter)null);
                }
                catch (Exception e) {
                    ScavePlugin.logError("Cannot open index file: "+filename, e);
                }
            }
        }
    }
}
