/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.model2.ScaveModelUtil;
import org.omnetpp.scave.python.ChartTemplate;


public class NewChartFromTemplateAction extends NewAnalysisItemAction {

    public NewChartFromTemplateAction(ChartTemplate template, boolean withEditDialog) {
        super(ScaveModelUtil.createChartFromTemplate(template), withEditDialog);
        setText("New " + template.getName());
        String iconName = "";
        switch (template.getChartType()) {
            case BAR:        iconName = ScaveImages.IMG_ETOOL16_NEWBARCHART;        break;
            case HISTOGRAM:  iconName = ScaveImages.IMG_ETOOL16_NEWHISTOGRAMCHART;  break;
            case LINE:       iconName = ScaveImages.IMG_ETOOL16_NEWLINECHART;       break;
            case MATPLOTLIB: iconName = ScaveImages.IMG_ETOOL16_NEWMATPLOTLIBCHART; break;
            case SCATTER:    iconName = ScaveImages.IMG_ETOOL16_NEWSCATTERCHART;    break;
        }
        setImageDescriptor(ScavePlugin.getImageDescriptor(iconName));
    }
}
