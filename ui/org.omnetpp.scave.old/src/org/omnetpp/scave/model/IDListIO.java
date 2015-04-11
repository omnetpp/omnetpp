/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.IProgressMonitor;
import org.xml.sax.ContentHandler;
import org.omnetpp.common.xml.XMLWriter;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.FileList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.plugin.ScavePlugin;
import static org.omnetpp.scave.model.IDListXMLConsts.*;


public class IDListIO {

    public static void save(IDList idlist, XMLWriter writer, IProgressMonitor progressMonitor) {
        try {
            ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
            Map<Run,String> map = new HashMap<Run,String>();
            int lastId = 1;

            // TODO use progressMonitor

            // write the list of files
            writer.writeStartElement(EL_FILES);
            FileList files = resultFileManager.getUniqueFiles(idlist);
            int size = (int)files.size();
            for (int i = 0; i < size; i++) {
                File file = files.get(i);
                String fileType = file.getFileType()==File.SCALAR_FILE ?
                        "scalar" : file.getFileType()==File.VECTOR_FILE ?
                                "vector" : "?";

                writer.writeStartElement(EL_FILE);
                writer.writeAttribute(ATT_FILENAME, file.getFilePath());
                writer.writeAttribute(ATT_TYPE, fileType);

                // write a list of runs in this file
                RunList runs = resultFileManager.getRunsInFile(file);
                int nRuns = (int)runs.size();
                for (int j=0; j<nRuns; j++) {
                    Run run = runs.get(j);
                    map.put(run,String.valueOf(lastId));
                    writer.writeStartElement(EL_RUN);
                    writer.writeAttribute(ATT_ID, String.valueOf(lastId++));
                    writer.writeAttribute(ATT_NAME, run.getRunName());
                    writer.writeEndElement(EL_RUN);
                }
                writer.writeEndElement(EL_FILE);
            }
            writer.writeEndElement(EL_FILES);

            // write the list of vectors/scalars
            writer.writeStartElement(EL_DATASET);
            size = (int)idlist.size();
            for (int i=0; i<size; i++) {
                ResultItem d = resultFileManager.getItem(idlist.get(i));
                writer.writeStartElement(EL_INCLUDE);
                writer.writeAttribute(ATT_RUN, map.get(d.getRun()));
                writer.writeAttribute(ATT_MOD, d.getModuleName());
                writer.writeAttribute(ATT_NAME, d.getName());
                writer.writeEndElement(EL_INCLUDE);
            }
            writer.writeEndElement(EL_DATASET);

        } catch (FileNotFoundException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static  Map<String,ContentHandler> getContentHandlers(IDList result, IProgressMonitor progressMonitor) {
        Map<String,ContentHandler> handlers = new HashMap<String,ContentHandler>(5);
        IDListSAXHandler idlistHandler = new IDListSAXHandler(result);
        handlers.put("files", idlistHandler);
        handlers.put("dataset", idlistHandler);
        return handlers;
    }
}
