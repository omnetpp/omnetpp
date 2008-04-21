package org.omnetpp.scave.editors.forms;

import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.ecore.EObject;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.Except;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.Param;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.SetOperation;

/**
 * Factory for <code>ScaveObjectEditForm</code>s.
 *
 * @author tomi
 */
public class ScaveObjectEditFormFactory {

	private static ScaveObjectEditFormFactory instance;
	
	public static ScaveObjectEditFormFactory instance() {
		if (instance == null)
			instance = new ScaveObjectEditFormFactory();
		return instance;
	}
	
	/**
	 * Creates a form containing all editable features of the object.
	 * @param object  the edited object
	 */
	public IScaveObjectEditForm createForm(EObject object, Map<String,Object> formParameters, ResultFileManager manager) {
		Assert.isTrue(object != null && object.eContainer() != null);
		return createForm(object, object.eContainer(), formParameters, manager);
	}

	/**
	 * Creates a form containing all editable features of the object.
	 * @param object the edited object
	 * @param parent the parent node of the object where it is placed or will be placed 
	 */
	public IScaveObjectEditForm createForm(EObject object, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
		
		if (object instanceof BarChart)
			return new BarChartEditForm((BarChart)object, parent, formParameters, manager);
		else if (object instanceof ScatterChart)
			return new ScatterChartEditForm((ScatterChart)object, parent, formParameters, manager);
		else if (object instanceof LineChart)
			return new LineChartEditForm((LineChart)object, parent, formParameters, manager);
		else if (object instanceof HistogramChart)
			return new HistogramChartEditForm((HistogramChart)object, parent, formParameters, manager);
		else if (object instanceof Chart)
			return new ChartEditForm((Chart)object, parent, formParameters, manager);
		else if (object instanceof ChartSheet)
			return new ChartSheetEditForm((ChartSheet)object, parent);
		else if (object instanceof Dataset)
			return new DatasetEditForm((Dataset)object, parent);
		else if (object instanceof InputFile)
			return new InputFileEditForm((InputFile)object, parent);
		else if (object instanceof ProcessingOp)
			return new ProcessingOperationEditForm((ProcessingOp)object, parent);
		else if (object instanceof SetOperation)
			return new SetOperationEditForm((SetOperation)object, parent, manager);
		else if (object instanceof Except)
			return new ExceptEditForm((Except)object, parent, manager);
		else if (object instanceof Group)
			return new GroupEditForm((Group)object, parent);
		else if (object instanceof Property)
			return new PropertyEditForm((Property)object, parent);
		else if (object instanceof Param)
			return new ParamEditForm((Param)object, parent);
		else
			throw new IllegalArgumentException("no edit form for "+object.getClass().getSimpleName());
	}
}
