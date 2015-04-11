/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

import java.util.HashMap;

import org.eclipse.jface.util.Assert;
import org.xml.sax.Attributes;
import org.xml.sax.helpers.DefaultHandler;

import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.plugin.ScavePlugin;

public class IDListSAXHandler extends DefaultHandler implements IDListXMLConsts {

    // collect result here
    private IDList result;

    // cached ref -- we'll need it all the time
    private ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;

    // the <file> element we're currently in
    private File currentFile;

    // maps run IDs to run references (because in the XML file, items refer to runs by an id)
    HashMap<String,Run> runIdToRunMap = new HashMap<String,Run>();

    private String skipedElement;

    public IDListSAXHandler(IDList result) {
        this.result = result;
    }

    public void startDocument() {
    }

    public void endDocument() {
    }

    public void startElement(String uri, String localName, String qName, Attributes atts) {
        if (skipedElement != null)
            return;

        //System.out.println("<" + qName + ">");
        if (qName.equals(EL_FILE)) {
            String fileName = atts.getValue(ATT_FILENAME);
            String type = atts.getValue(ATT_TYPE);
            if (fileName==null || type==null)
                throw new RuntimeException("invalid file format: missing or invalid XML attribute");
            //System.out.println("FILE "+fileName);
            //TODO check beforehand if file exists, and offer user to point to it via file dialog
            java.io.File f = new java.io.File(fileName);
            if (!f.canRead())
                throw new RuntimeException("embedded file '"+fileName+"' doesn't exist or there is no permission to read it");
            if ("scalar".equals(type))
                currentFile = resultFileManager.loadScalarFile(fileName);
            else if ("vector".equals(type))
                currentFile = resultFileManager.loadVectorFile(fileName);
            else
                throw new RuntimeException("invalid file format: missing or invalid XML attribute");

        }
        else if (qName.equals(EL_RUN)) {
            Assert.isNotNull(currentFile);
            String runName = atts.getValue(ATT_NAME);
            String runId = atts.getValue(ATT_ID);
            if (runName==null || runId==null)
                throw new RuntimeException("invalid file format: missing or invalid XML attribute");
            //System.out.println("RUN " + runName);
            Run run = resultFileManager.getRunByName(currentFile, runName);
            if (run==null)
                throw new RuntimeException("file '"+currentFile.getFilePath()+"' doesn't contain a run named '"+runName+"'");
            runIdToRunMap.put(runId, run);
        }
        else if (qName.equals(EL_INCLUDE)) {
            String runId = atts.getValue(ATT_RUN);
            String moduleName = atts.getValue(ATT_MOD);
            String name = atts.getValue(ATT_NAME);
            if (runId==null || moduleName==null || name==null)
                throw new RuntimeException("invalid file format: missing or invalid XML attribute");
            //System.out.println("ITEM: run="+runId+" module="+moduleName+" name="+name);
            Run run = runIdToRunMap.get(runId);
            if (run==null)
                throw new RuntimeException("invalid file format: no run with id="+runId);
            long id = resultFileManager.getItemByName(run, moduleName, name);
            if (id==0)
                throw new RuntimeException("invalid file format: run id="+runId+
                        " does not contain item module='"+moduleName+"' name='"+name+"'");
            result.add(id);
        }
        else {
            if (qName!=EL_SCAVE && qName!=EL_DATASET && qName!=EL_INCLUDE && qName!=EL_FILES && qName!=EL_FILE && qName!=EL_RUN)
                //throw new RuntimeException("invalid file format: unknown XML tag '"+qName+"'");
                skipedElement = qName;
        }
    }

    public void endElement(String uri, String localName, String qName) {
        //System.out.println("</" + qName + ">");
        if (skipedElement != null && skipedElement.equals(qName))
            skipedElement = null;
    }

    public void characters(char[] ch, int offset, int length) {
        //System.out.println("(characters:" + new String(ch, offset, length)+")");
    }
}