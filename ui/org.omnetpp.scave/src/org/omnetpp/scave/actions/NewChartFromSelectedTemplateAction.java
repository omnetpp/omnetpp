/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.NewChartFromTemplateDialog;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model.commands.AddChartCommand;
import org.omnetpp.scave.model.commands.ICommand;
import org.omnetpp.scave.model2.ScaveModelUtil;


public class NewChartFromSelectedTemplateAction extends AbstractScaveAction {

    public NewChartFromSelectedTemplateAction() {
        setText("New Chart");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_NEWBARCHART));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        NewChartFromTemplateDialog dialog = new NewChartFromTemplateDialog(Display.getCurrent().getActiveShell(), editor);
        if (dialog.open() != Dialog.OK)
            return;

        ChartTemplate template = dialog.getSelectedTemplate();
        if (template == null)
            return;

        Chart chart = ScaveModelUtil.createChartFromTemplate(template);
        chart.assignNewId();
        ICommand command = new AddChartCommand(editor.getAnalysis(), chart);
        editor.getChartsPage().getCommandStack().execute(command);
        editor.showAnalysisItem(chart);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }
}
