/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.treeproviders;

import static org.apache.commons.lang3.StringUtils.defaultIfEmpty;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.provider.IWrapperItemProvider;
import org.eclipse.jface.viewers.IColorProvider;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.Markers;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.MatplotlibChart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Label provider for model objects. We use this instead
 * of the EMF-generated default label provider.
 *
 * @author andras
 */
public class ScaveModelLabelProvider extends LabelProvider implements IColorProvider {

    ILabelProvider fallback;

    public ScaveModelLabelProvider(ILabelProvider fallback) {
        this.fallback = fallback;
    }

    @Override
    public void dispose() {
        if (fallback != null)
            fallback.dispose();
    }

    @Override
    public Image getImage(Object element) {
        return fallback != null ? fallback.getImage(element) : null;
    }

    @Override
    public String getText(Object element) {
        if (element instanceof Analysis) {
            return "Analysis";
        }
        else if (element instanceof Inputs) {
            return "Inputs";
        }
        else if (element instanceof InputFile) {
            InputFile o = (InputFile) element;
            return "file "+defaultIfEmpty(o.getName(), "<nothing>");
        }
        else if (element instanceof BarChart) {
            Chart o = (Chart) element;
            return "bar chart "+defaultIfEmpty(o.getName(), "<unnamed>");
        }
        else if (element instanceof LineChart) {
            Chart o = (Chart) element;
            return "line chart "+defaultIfEmpty(o.getName(), "<unnamed>");
        }
        else if (element instanceof HistogramChart) {
            Chart o = (Chart) element;
            return "histogram chart "+defaultIfEmpty(o.getName(), "<unnamed>");
        }
        else if (element instanceof ScatterChart) {
            Chart o = (Chart) element;
            return "scatter chart "+defaultIfEmpty(o.getName(), "<unnamed>");
        }
        else if (element instanceof MatplotlibChart) {
            Chart o = (Chart) element;
            return "python chart "+defaultIfEmpty(o.getName(), "<unnamed>");
        }
        else if (element instanceof Chart) {
            throw new IllegalArgumentException("unrecognized chart type");
        }
        else if (element instanceof Property) {
            Property o = (Property) element;
            return defaultIfEmpty(o.getName(), "<undefined>")+" = "+defaultIfEmpty(o.getValue(), "");
        }
        else if (element instanceof IWrapperItemProvider) {
            return getText(((IWrapperItemProvider)element).getValue());
        }
        return element.toString(); // fallback
    }

    public String getTooltipText(Object element) {
        if (element instanceof Chart) {
            Chart item = (Chart)element;
            IFile file = ScaveModelUtil.getFileOfEObject(item);
            if (file != null && hasError(file, item))
                return "Errors, see Problems View";
        }
        return null;
    }

    @Override
    public Color getForeground(Object element) {
        if (element instanceof Chart) {
            Chart item = (Chart)element;
            IFile file = ScaveModelUtil.getFileOfEObject(item);
            if (file != null && hasError(file, item))
                return ColorFactory.RED;
        }
        return null;
    }

    @Override
    public Color getBackground(Object element) {
        return null;
    }

    private boolean hasError(IFile file, EObject object) {
        try {
            IMarker problems[] = file.findMarkers(Markers.ANALYSISFILE_PROBLEMMARKER_ID, true, IResource.DEPTH_ZERO);
            for (IMarker problem : problems) {
                if (problem.getAttribute(Markers.EOBJECT_MARKERATTR_ID) == object)
                    return true;
            }
            return false;
        } catch (CoreException e) {
            return false;
        }
    }
}
