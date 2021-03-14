/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.Comparator;
import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.omnetpp.common.properties.BasePropertySource;
import org.omnetpp.scave.model.Chart;

/**
 * Property provider for Chart objects.
 *
 * @author andras
 */
public class ChartPropertySource extends BasePropertySource {
    private final Chart chart;

    public ChartPropertySource(Chart chart) {
        this.chart = chart;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        List<String> names = chart.getPropertyNames();
        names.sort(Comparator.naturalOrder());

        IPropertyDescriptor[] descriptors = new IPropertyDescriptor[names.size()];
        for (int i = 0; i < descriptors.length; i++) {
            String name = names.get(i);
            descriptors[i] = new ReadonlyPropertyDescriptor(name, name, null);
        }
        return descriptors;
    }

    @Override
    public Object getPropertyValue(Object id) {
        String name = id.toString();
        return chart.getPropertyValue(name);
    }
}