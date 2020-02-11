/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.DialogPage;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class SaveChartAsTemplateAction extends AbstractScaveAction {
    public SaveChartAsTemplateAction() {
        setText("Save as Template");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_ETOOL_SAVEAS_EDIT));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) {
        Chart chart = ScaveModelUtil.getChartFromSingleSelection(selection);

        String filenames;

        IProject currentProject = editor.getAnfFile().getProject();
        IFolder templatesFolder = currentProject.getFolder("charttemplates");
        File templatesDir = templatesFolder.getLocation().toFile();

        InputDialog dialog = new InputDialog(Display.getCurrent().getActiveShell(),
                "Save Chart as Template",
                "Enter filename base below. Files will be created in the analysis file's project, " +
                "under " + templatesFolder.getFullPath().toString()+ ".",
                chart.getTemplateID() + "_" + chart.getId(), null);
        if (dialog.open() != Dialog.OK)
            return;

        String newTemplateId = dialog.getValue();

        if (templatesDir.exists() && !templatesDir.isDirectory())
            throw new RuntimeException("\"charttemplates\" exists in the project, but is not a directory");

        if (!templatesDir.exists())
            templatesDir.mkdirs();

        String propertiesFileName = newTemplateId + ".properties";
        File propertiesFile = new File(templatesDir.getAbsolutePath() + "/" + propertiesFileName);
        String scriptFileName = newTemplateId + ".py";
        File scriptFile = new File(templatesDir.getAbsolutePath() + "/" + scriptFileName);

        filenames = propertiesFileName + ",\n" + scriptFileName;

        if (propertiesFile.exists())
            throw new RuntimeException(propertiesFile.getAbsolutePath() + " already exists");
        if (scriptFile.exists())
            throw new RuntimeException(scriptFile.getAbsolutePath() + " already exists");

        try {
            scriptFile.createNewFile();
            FileWriter sfw = new FileWriter(scriptFile);
            sfw.write(chart.getScript());
            sfw.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }


        try {
            propertiesFile.createNewFile();
            FileWriter pfw = new FileWriter(propertiesFile);

            pfw.write("id = " + newTemplateId + "\n");
            pfw.write("name = " + chart.getName() + "\n");
            pfw.write("type = " + chart.getType().toString() + "\n");
            pfw.write("scriptFile = " + scriptFileName + "\n");
            pfw.write("icon = " + chart.getIconPath() + "\n");
            // omitting toolbarOrder and resultTypes
            pfw.write("\n");

            pfw.write("propertyNames = ");
            boolean first=true;
            for (Property p : chart.getProperties()) {
                if (!first)
                    pfw.write(",\\\n    ");
                pfw.write(p.getName());
                if (p.getValue() != null && !p.getName().equals("filter"))
                    pfw.write(":" + p.getValue()); // TODO: what if the value contains a comma? should quote...
                first = false;
            }

            pfw.write("\n\n");

            int i = 0;
            for (DialogPage dp : chart.getDialogPages()) {

                String xswtFileName = newTemplateId + "_" + dp.id + ".xswt";

                filenames += ",\n" + xswtFileName;

                File xswtFile = new File(templatesDir.getAbsoluteFile() + "/" + xswtFileName);

                if (xswtFile.exists())
                    throw new RuntimeException(xswtFile.getAbsolutePath() + " already exists");

                FileWriter xfw = new FileWriter(xswtFile);
                xfw.write(dp.xswtForm);
                xfw.close();

                pfw.write("\n");

                pfw.write("dialogPage." + i + ".id = " + dp.id + "\n");
                pfw.write("dialogPage." + i + ".label = " + dp.label + "\n");
                pfw.write("dialogPage." + i + ".xswtFile = " + xswtFileName + "\n");

                i++;
            }
            pfw.write("\n\n");

            pfw.close();

        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        MessageDialog.openInformation(editor.getSite().getShell(), "Chart saved as template",
                "The selected chart was exported as a template, generating the following files in the \"" +
                templatesDir.getAbsolutePath() + "\" directory:\n\n" + filenames + "\n\n" +
                "You can create new charts from it using the \"New\" submenu of the context menu on the \"Charts\" page."
                );
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return ScaveModelUtil.getChartFromSingleSelection(selection) != null;
    }
}
