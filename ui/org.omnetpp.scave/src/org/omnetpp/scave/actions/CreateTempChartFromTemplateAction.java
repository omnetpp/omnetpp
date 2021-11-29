/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model2.ResultSelectionFilterGenerator;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Creates a temporary chart from the selection on the BrowseDataPage, using
 * the given template, then opens it.
 */
// TODO consider merging/subclassing with PlotAction?
public class CreateTempChartFromTemplateAction extends AbstractScaveAction {

    ChartTemplate template;

    public CreateTempChartFromTemplateAction(ChartTemplate template) {
        this.template = template;
        setText("Plot using " + template.getName());
        setToolTipText("Plot using " + template.getName());

        String iconName = template.getMenuIconPath();

        if (iconName == null || iconName.isEmpty())
            switch (template.getChartType()) {
                case BAR:        iconName = ScaveImages.IMG_ETOOL16_NEWBARCHART;        break;
                case HISTOGRAM:  iconName = ScaveImages.IMG_ETOOL16_NEWHISTOGRAMCHART;  break;
                case LINE:       iconName = ScaveImages.IMG_ETOOL16_NEWLINECHART;       break;
                case MATPLOTLIB: iconName = ScaveImages.IMG_ETOOL16_NEWMATPLOTLIBCHART; break;
            }
        setImageDescriptor(ScavePlugin.getImageDescriptor(iconName));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        IDListSelection idListSelection = (IDListSelection)selection;

        Chart chart = ScaveModelUtil.createChartFromTemplate(template);
        editor.getChartTemplateRegistry().markTemplateUsage(template);

        String filter = ResultSelectionFilterGenerator.makeFilterForIDListSelection(idListSelection);
        int types = idListSelection.getSource().getType().getItemTypes();
        boolean includeFields = editor.getBrowseDataPage().getScalarsPanel().getShowFieldsAsScalars();
        editor.getFilterCache().putFilterResult(types, filter, includeFields, idListSelection.getIDList());
        chart.setPropertyValue("filter", filter);
        chart.setPropertyValue("include_fields", "" + includeFields);
        chart.setTemporary(true);
        editor.openPage(chart);
    }


    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return selection instanceof IDListSelection && !selection.isEmpty();
    }
}
