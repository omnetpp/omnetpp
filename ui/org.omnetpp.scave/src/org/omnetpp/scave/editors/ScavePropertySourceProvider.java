/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.concurrent.Callable;

import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.common.properties.MergedPropertySource;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.scave.charting.properties.PlotProperties;
import org.omnetpp.scave.charting.properties.MatplotlibChartProperties;
import org.omnetpp.scave.charting.properties.LinePlotProperties;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.ChartType;
import org.omnetpp.scave.model.commands.CommandStack;
import org.omnetpp.scave.model2.ChartLine;
import org.omnetpp.scave.model2.ResultFilePayload;
import org.omnetpp.scave.model2.ResultItemRef;
import org.omnetpp.scave.model2.RunPayload;

/**
 * Provides properties of scave model objects and charts.
 *
 * @author tomi
 */
public class ScavePropertySourceProvider implements IPropertySourceProvider {

    ResultFileManager manager;

    public ScavePropertySourceProvider(ResultFileManager manager) {
        this.manager = manager;
    }

    @Override
    public IPropertySource getPropertySource(final Object object) {
        if (object instanceof GenericTreeNode)
            return getPropertySource(((GenericTreeNode)object).getPayload());
        else if (object instanceof Chart)
            return ChartVisualProperties.createPropertySource((Chart)object);
        else if (object instanceof PropertySource)
            return (PropertySource)object;
        else if (object instanceof ChartLine) {
            ChartLine lineID = (ChartLine)object;
            // TODO
//            ChartProperties properties = ChartProperties.createPropertySource(lineID.getChart(), manager);
//            if (properties instanceof VectorChartProperties)
//                return ((VectorChartProperties)properties).getLineProperties(lineID.getKey());
        }
        else if (object instanceof ResultItemRef)
            return new ResultItemPropertySource((ResultItemRef)object);
        else if (object instanceof ResultFilePayload)
            return new ResultFilePropertySource(manager.getFile(((ResultFilePayload)object).getFilePath()));
        else if (object instanceof RunPayload)
            return ResultFileManager.callWithReadLock(manager, new Callable<RunPropertySource>() {
                @Override
                public RunPropertySource call() throws Exception {
                    return new RunPropertySource(manager.getRunByName(((RunPayload)object).getRunName()));
                }
            });

        return null; // TODO ?
    }
}
