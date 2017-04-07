/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.treeproviders;

import static org.apache.commons.lang3.StringUtils.abbreviate;
import static org.apache.commons.lang3.StringUtils.defaultIfEmpty;
import static org.apache.commons.lang3.StringUtils.isEmpty;

import java.util.List;

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
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Apply;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.ChartSheets;
import org.omnetpp.scave.model.Compute;
import org.omnetpp.scave.model.ComputeScalar;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.Datasets;
import org.omnetpp.scave.model.Deselect;
import org.omnetpp.scave.model.Discard;
import org.omnetpp.scave.model.Except;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.Param;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.Select;
import org.omnetpp.scave.model.SetOperation;
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

    public void dispose() {
        if (fallback != null)
            fallback.dispose();
    }

    public Image getImage(Object element) {
        return fallback != null ? fallback.getImage(element) : null;
    }

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
        else if (element instanceof ChartSheets) {
            return "Chart Sheets";
        }
        else if (element instanceof ChartSheet) {
            ChartSheet o = (ChartSheet) element;
            return "chart sheet "+defaultIfEmpty(o.getName(), "<unnamed>")+" ("+o.getCharts().size()+" charts)";
        }
        else if (element instanceof Datasets) {
            return "Datasets";
        }
        else if (element instanceof Dataset) {
            Dataset o = (Dataset) element;
            String res = "dataset "+defaultIfEmpty(o.getName(), "<unnamed>");
            if (o.getBasedOn()!=null)
                res += " based on dataset "+defaultIfEmpty(o.getBasedOn().getName(), "<unnamed>");
            return res;
        }
        else if (element instanceof SetOperation) {
            // Add, Discard, Select, Deselect
            SetOperation o = (SetOperation) element;

            // "select..."
            String res = "";
            if (element instanceof Add)
                res = "add";
            else if (element instanceof Discard)
                res = "discard";
            else if (element instanceof Select)
                res = "select";
            else if (element instanceof Deselect)
                res = "deselect";
            else if (element instanceof Except)
                res = "except";
            else
                res = "???";

            res += " " + (o.getType()==null ? "???" : o.getType().getName())+"s: ";
            res += defaultIfEmpty(o.getFilterPattern(), "all");

            if (o.getSourceDataset()!=null)
                res += " from dataset "+defaultIfEmpty(o.getSourceDataset().getName(), "<unnamed>");

            return res;
        }
        else if (element instanceof Except) {
            Except o = (Except) element;
            return "except " + defaultIfEmpty(o.getFilterPattern(), "all");
        }
        else if (element instanceof ProcessingOp) {
            ProcessingOp o = (ProcessingOp) element;
            StringBuilder sb = new StringBuilder();

            if (element instanceof Apply)
                sb.append("apply");
            else if (element instanceof Compute)
                sb.append("compute");
            else
                sb.append("<unknown operation>");

            sb.append(' ').append(defaultIfEmpty(o.getOperation(), "<undefined>"));

            List<Param> params = o.getParams();
            if (!params.isEmpty()) sb.append('(');
            boolean firstIteration = true;
            for (Param param : params) {
                if (!firstIteration)
                    sb.append(',');
                else
                    firstIteration = false;
                sb.append(defaultIfEmpty(param.getName(), "<undefined>")).append('=').append(defaultIfEmpty(param.getValue(), ""));
            }
            if (!params.isEmpty()) sb.append(')');


            return sb.toString();
        }
        else if (element instanceof ComputeScalar) {
            ComputeScalar o = (ComputeScalar) element;
            StringBuilder sb = new StringBuilder();
            sb.append("compute scalar: ");
            if (!isEmpty(o.getScalarName()))
                sb.append(o.getScalarName());
            else
                sb.append("<unnamed>");
            if (!isEmpty(o.getValueExpr()))
                sb.append(" = ").append(o.getValueExpr());
            return abbreviate(sb.toString(), 200);
        }
        else if (element instanceof Group) {
            Group o = (Group) element;
            return isEmpty(o.getName()) ? "group" : "group "+o.getName();
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
        else if (element instanceof Chart) {
            throw new IllegalArgumentException("unrecognized chart type");
        }
        else if (element instanceof Param) {
            Param o = (Param) element;
            return defaultIfEmpty(o.getName(), "<undefined>")+" = "+defaultIfEmpty(o.getValue(), "");
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
        if (element instanceof DatasetItem) {
            DatasetItem datasetItem = (DatasetItem)element;
            IFile file = ScaveModelUtil.getFileOfEObject(datasetItem);
            if (file != null && hasError(file, datasetItem))
                return "Errors, see Problems View";
        }
        return null;
    }

    public Color getForeground(Object element) {
        if (element instanceof DatasetItem) {
            DatasetItem datasetItem = (DatasetItem)element;
            IFile file = ScaveModelUtil.getFileOfEObject(datasetItem);
            if (file != null && hasError(file, datasetItem))
                return ColorFactory.RED;
        }
        return null;
    }

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
