/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2.provider;

import java.util.Collection;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class DatasetItemProvider extends org.omnetpp.scave.model.provider.DatasetItemProvider {

    public DatasetItemProvider(AdapterFactory adapterFactory) {
        super(adapterFactory);
    }

    /**
     * When a chart added to the dataset, which does not have chart sheet,
     * add the chart to the default chart sheet.
     * The default chart sheet is created if it did not exist.
     */
    @Override
    protected Command createAddCommand(EditingDomain domain, EObject owner, EStructuralFeature feature, Collection<?> collection, int index) {
        Command addCommand = super.createAddCommand(domain, owner, feature, collection, index);

        Collection<Chart> charts = ScaveModelUtil.collectUnreferencedCharts(collection);
        if (charts.size() > 0 && owner.eResource() != null) {
            CompoundCommand command = new CompoundCommand(addCommand.getLabel(), addCommand.getDescription());
            command.append(addCommand);
            ChartSheet chartsheet = ScaveModelUtil.getOrCreateDefaultChartSheet(domain, command, owner.eResource());
            command.append(
                AddCommand.create(
                    domain,
                    chartsheet,
                    ScaveModelPackage.eINSTANCE.getChartSheet_Charts(),
                    charts));
            return command;
        }
        else
            return addCommand;
    }
}
