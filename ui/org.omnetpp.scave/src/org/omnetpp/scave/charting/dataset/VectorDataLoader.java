/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.dataset;

import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.EntryVector;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFile.FileType;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engineext.IndexedVectorFileReaderEx;

public class VectorDataLoader {
    public static XYVector[] getDataOfVectors(ResultFileManager manager, IDList idlist, IProgressMonitor progressMonitor) {

        XYVector[] vectors = new XYVector[idlist.size()];

        progressMonitor.beginTask("Loading vectors", idlist.size());

        // TODO: optimize! Even if some vectors are in the same file, that file might be read multiple times,
        // once per every needed vector in it!
        for (int i = 0; i < idlist.size(); ++i) {
            long id = idlist.get(i);

            VectorResult vec = manager.getVector(id);
            ResultFile f = vec.getFile();
            if (f.getFileType() == FileType.FILETYPE_OMNETPP) {
                IndexedVectorFileReaderEx reader = new IndexedVectorFileReaderEx(f.getFileSystemFilePath(), vec.getVectorId());
                EntryVector out = new EntryVector();
                reader.collectEntriesInSimtimeInterval(new BigDecimal(0), new BigDecimal(1000), out);
                vectors[i] = new XYVector((int)out.size());
                for (int k = 0; k < out.size(); ++k) {
                    vectors[i].x[k] = out.get(k).getSimtime().doubleValue();
                    vectors[i].y[k] = out.get(k).getValue();
                }
            }

            progressMonitor.worked(1);
        }
        progressMonitor.done();
        return vectors;
    }
}
