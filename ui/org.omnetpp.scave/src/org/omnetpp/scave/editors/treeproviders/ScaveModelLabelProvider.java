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
import org.eclipse.jface.viewers.IColorProvider;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.scave.Markers;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.AnalysisObject;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.Property;

/**
 * Label provider for model objects.
 *
 * @author andras
 */
public class ScaveModelLabelProvider extends LabelProvider implements IColorProvider {

    @Override
    public Image getImage(Object element) {
        return null;
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
        else if (element instanceof Chart) {
            Chart chart = (Chart)element;
            return chart.getType().toString().toLowerCase() + " chart "+defaultIfEmpty(chart.getName(), "<unnamed>");
        }
        else if (element instanceof Property) {
            Property o = (Property) element;
            return defaultIfEmpty(o.getName(), "<undefined>")+" = "+defaultIfEmpty(o.getValue(), "");
        }
        return element.toString(); // fallback
    }

    public String getTooltipText(Object element) {
        if (element instanceof Chart) {
            Chart item = (Chart)element;

            // TODO
//            IFile file = ScaveModelUtil.getFileOfEObject(item);
//            if (file != null && hasError(file, item))
//                return "Errors, see Problems View";
        }
        return null;
    }

    @Override
    public Color getForeground(Object element) {
        if (element instanceof Chart) {
            Chart item = (Chart)element;
            // TODO
//            IFile file = ScaveModelUtil.getFileOfEObject(item);
//            if (file != null && hasError(file, item))
//                return ColorFactory.RED;
        }
        return null;
    }

    @Override
    public Color getBackground(Object element) {
        return null;
    }

    private boolean hasError(IFile file, AnalysisObject object) {
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
