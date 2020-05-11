/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.analysismodel;

import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model2.ScaveModelUtil;


public class NewChartFromTemplateAction extends NewAnalysisItemAction {

    public NewChartFromTemplateAction(ChartTemplate template) {
        this(template, true);
    }

    public NewChartFromTemplateAction(ChartTemplate template, boolean prefix) {
        super(ScaveModelUtil.createChartFromTemplate(template));
        setText(prefix ? "New " + template.getName() : template.getName());

        String iconName = template.getToolbarIconPath();

        if (iconName == null || iconName.isEmpty())
            switch (template.getChartType()) {
                case BAR:        iconName = ScaveImages.IMG_ETOOL16_NEWBARCHART;        break;
                case HISTOGRAM:  iconName = ScaveImages.IMG_ETOOL16_NEWHISTOGRAMCHART;  break;
                case LINE:       iconName = ScaveImages.IMG_ETOOL16_NEWLINECHART;       break;
                case MATPLOTLIB: iconName = ScaveImages.IMG_ETOOL16_NEWMATPLOTLIBCHART; break;
            }
        setImageDescriptor(ScavePlugin.getImageDescriptor(iconName));
    }
}
