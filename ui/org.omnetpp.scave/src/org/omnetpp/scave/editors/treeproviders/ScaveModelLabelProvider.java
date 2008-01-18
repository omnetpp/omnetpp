package org.omnetpp.scave.editors.treeproviders;

import static org.apache.commons.lang.StringUtils.defaultIfEmpty;
import static org.apache.commons.lang.StringUtils.isEmpty;

import org.eclipse.emf.edit.provider.IWrapperItemProvider;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Apply;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.ChartSheets;
import org.omnetpp.scave.model.Compute;
import org.omnetpp.scave.model.Dataset;
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
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.Select;
import org.omnetpp.scave.model.SetOperation;

/**
 * Label provider for model objects. We use this instead 
 * of the EMF-generated default label provider.
 *  
 * @author andras
 */
public class ScaveModelLabelProvider extends LabelProvider {
	
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
		else if (element instanceof Apply) {
			Apply o = (Apply) element;
			return "apply "+defaultIfEmpty(o.getOperation(), "<undefined>");
		}
		else if (element instanceof Compute) {
			Compute o = (Compute) element;
			return "compute "+defaultIfEmpty(o.getOperation(), "<undefined>");
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
}
