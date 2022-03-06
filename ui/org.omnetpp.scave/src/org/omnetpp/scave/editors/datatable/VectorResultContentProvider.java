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
import org.omnetpp.scave.editors.ResultFileException;
import org.omnetpp.scave.engine.FileFingerprint;
import org.omnetpp.scave.engine.IVectorDataReader;
import org.omnetpp.scave.engine.IVectorDataReader.Adapter;
import org.omnetpp.scave.engine.IndexedVectorFileReader;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.SqliteResultFileUtils;
import org.omnetpp.scave.engine.SqliteVectorDataReader;
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

    private IVectorDataReader reader;
    private int vectorId;

    private ResultFileException wrap(RuntimeException e) {
        return new ResultFileException(e.getMessage(), e);
    }

    public VectorDatum getApproximateElementAt(double percentage) {
        try {
            return reader == null ? null : reader.getEntryBySerial(vectorId, (int)(percentage * reader.getNumberOfEntries(vectorId)));
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
    }

    public long getApproximateNumberOfElements() {
        try {
            return reader == null ? 0 : reader.getNumberOfEntries(vectorId);
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
    }

    public double getApproximatePercentageForElement(VectorDatum element) {
        try {
            return reader == null ? 0.0 : ((double)element.getSerial()) / reader.getNumberOfEntries(vectorId);
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
    }

    public int compare(VectorDatum element1, VectorDatum element2) {
        try {
            return element1.getSerial() - element2.getSerial();
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
    }

    public long getDistanceToElement(VectorDatum sourceElement, VectorDatum targetElement, long limit) {
        if (reader == null)
            return 0;
        try {
            long delta = targetElement.getSerial() - sourceElement.getSerial();
            if (Math.abs(delta) > Math.abs(limit) || Math.signum(delta) != Math.signum(limit))
                return limit;
            else
                return delta;
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
    }

    public long getDistanceToFirstElement(VectorDatum element, long limit) {
        try {
            return reader == null ? 0 : Math.min(element.getSerial(), limit);
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
    }

    public long getDistanceToLastElement(VectorDatum element, long limit) {
        if (reader == null)
            return 0;
        try {
            int lastSerial = reader.getNumberOfEntries(vectorId) - 1;
            return Math.min(lastSerial - element.getSerial(), limit);
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
    }

    public VectorDatum getFirstElement() {
        try {
            return reader == null ? null : reader.getEntryBySerial(vectorId, 0);
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
    }

    public VectorDatum getLastElement() {
        try {
            return reader == null ? null : reader.getEntryBySerial(vectorId, reader.getNumberOfEntries(vectorId) - 1);
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
    }

    public VectorDatum getNeighbourElement(VectorDatum element, long distance) {
        try {
            return reader == null ? null : reader.getEntryBySerial(vectorId, (int)(element.getSerial() + distance));
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
    }

    public VectorDatum getClosestElement(VectorDatum element) {
        return element;
    }

    public VectorDatum getElementBySerial(int serial) {
        try {
            return reader == null ? null : reader.getEntryBySerial(vectorId, serial);
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
    }

    public VectorDatum getElementBySimulationTime(BigDecimal time, boolean after) {
        try {
            return reader == null ? null : reader.getEntryBySimtime(vectorId, time, after);
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
    }

    public VectorDatum getElementByEventNumber(long eventNumber, boolean after) {
        try {
            return reader == null ? null : reader.getEntryByEventnum(vectorId, eventNumber, after);
        }
        catch (RuntimeException e) {
            throw wrap(e);
        }
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
                String filename = vector.getFile().getFileSystemFilePath();
                FileFingerprint fingerprint = vector.getFile().getFingerprint();

                try {
                    if (SqliteResultFileUtils.isSqliteFile(filename))
                        reader = new SqliteVectorDataReader(filename, true, (Adapter)null, fingerprint);
                    else
                        reader = new IndexedVectorFileReader(filename, true, (Adapter)null, fingerprint);
                }
                catch (RuntimeException e) {
                    throw wrap(e);
                }
            }
        }
    }
}
