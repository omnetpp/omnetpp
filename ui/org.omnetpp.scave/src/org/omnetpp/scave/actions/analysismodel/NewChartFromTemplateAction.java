/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.analysismodel;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model.commands.AddChartCommand;
import org.omnetpp.scave.model.commands.ICommand;
import org.omnetpp.scave.model2.ScaveModelUtil;


public class NewChartFromTemplateAction extends AbstractScaveAction {
    private ChartTemplate template;

    public NewChartFromTemplateAction(ChartTemplate template) {
        this(template, true);
    }

    public NewChartFromTemplateAction(ChartTemplate template, boolean prefix) {
        this.template = template;

        setText(prefix ? "New " + template.getName() : template.getName());

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
        Chart chart = ScaveModelUtil.createChartFromTemplate(template, editor.makeNameForNewChart(template));
        ICommand command = new AddChartCommand(editor.getCurrentFolder(), chart);
        editor.getChartsPage().getCommandStack().execute(command);
        editor.showAnalysisItem(chart);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }

}
