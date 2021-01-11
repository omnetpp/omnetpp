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
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartTemplateGalleryDialog;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model2.ResultSelectionFilterGenerator;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Creates a temporary chart by letting the user select from the Template Gallery dialog,
 * then opens it.
 */
public class CreateTempChartFromGalleryAction extends AbstractScaveAction {
    private int selectionItemTypes;

    public CreateTempChartFromGalleryAction(int selectionItemTypes) {
        setText("Choose from Template Gallery...");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_NEWBARCHART));
        this.selectionItemTypes = selectionItemTypes;
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        IDListSelection idListSelection = (IDListSelection)selection;

        ChartTemplateGalleryDialog dialog = new ChartTemplateGalleryDialog(Display.getCurrent().getActiveShell(), editor, selectionItemTypes);
        if (dialog.open() != Dialog.OK)
            return;

        ChartTemplate template = dialog.getSelectedTemplate();
        if (template == null)
            return;

        Chart chart = ScaveModelUtil.createChartFromTemplate(template);
        editor.getChartTemplateRegistry().markTemplateUsage(template);

        String filter = ResultSelectionFilterGenerator.makeFilterForIDListSelection(idListSelection);
        int types = idListSelection.getSource().getType().getItemTypes();
        editor.getFilterCache().putFilterResult(types, filter, idListSelection.getIDList());
        chart.setPropertyValue("filter", filter);
        chart.setTemporary(true);
        editor.openPage(chart);
    }


    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return selection instanceof IDListSelection && !selection.isEmpty();
    }
}
