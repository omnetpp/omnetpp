/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2.provider;

import java.util.Collection;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.edit.command.CommandParameter;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.SelectDeselectOp;

/**
 * Item provider for line charts.
 * It sets the default type of new Select/Deselect children according to 'vector'.
 *
 * @author tomi
 */
public class LineChartItemProvider extends
        org.omnetpp.scave.model.provider.LineChartItemProvider {

    public LineChartItemProvider(AdapterFactory adapterFactory) {
        super(adapterFactory);
    }

    /**
     * Set the default type of new Select/Deselect children to 'Vector'.
     */
    @Override
    protected void collectNewChildDescriptors(Collection<Object> newChildDescriptors, Object object) {
        super.collectNewChildDescriptors(newChildDescriptors, object);

        if (object instanceof LineChart) {
            for (Object descriptor : newChildDescriptors) {
                CommandParameter param = (CommandParameter)descriptor;
                if (param.value instanceof SelectDeselectOp) {
                    SelectDeselectOp op = (SelectDeselectOp)param.value;
                    op.setType(ResultType.VECTOR_LITERAL);
                }
            }
        }
    }
}
