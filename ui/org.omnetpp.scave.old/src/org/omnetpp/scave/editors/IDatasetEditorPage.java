/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.io.IOException;
import java.util.Map;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.xml.XMLWriter;
import org.xml.sax.ContentHandler;

public interface IDatasetEditorPage {

    String getPageTitle();

    void setEditor(DatasetEditor editor);

    void init();

    void dispose();

    Control createPageControl(Composite parent);

    void finalizePage();

    void activate();

    void deactivate();

    void save(XMLWriter writer, IProgressMonitor progressMonitor) throws IOException;

    Map<String,ContentHandler> getLoader(IProgressMonitor progressMonitor);
}
