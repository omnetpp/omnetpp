/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.wizard;

import org.eclipse.jface.resource.ImageDescriptor;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.ExporterFactory;
import org.omnetpp.scave.engine.ScaveExport;

public class OctaveExportWizard extends AbstractExportWizard {

    public static final String ID = "org.omnetpp.scave.wizard.OctaveExportWizard";

    public OctaveExportWizard() {
        setDialogSettings(UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName()));
    }

    @Override
    public void addPages() {
        page = new OctaveExportPage("Octave export", "Export in Octave text format", null);
        addPage(page);
    }

    @Override
    protected ScaveExport createExporter() {
        return ExporterFactory.createExporter("octave");
    }

    /*---------------------
     *        Pages
     *---------------------*/

    static class OctaveExportPage extends ExportWizardPage
    {
        protected OctaveExportPage(String pageName, String title,
                ImageDescriptor titleImage) {
            super(pageName, title, titleImage);
        }

        @Override
        protected String[] getFileDialogFilterExtensions() {
            return new String[] { "*.octave", "*.*" };
        }
    }
}
