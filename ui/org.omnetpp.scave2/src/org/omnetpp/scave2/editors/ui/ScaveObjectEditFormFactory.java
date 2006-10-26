package org.omnetpp.scave2.editors.ui;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.AddDiscardOp;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model.SetOperation;

/**
 * Factory for <code>ScaveObjectEditForm</code>s.
 * 
 * XXX This factory returns a generated form,
 *     more specific forms will be implemented in the future.
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
	public IScaveObjectEditForm createForm(EObject object, ResultFileManager manager) {
		
		if (object instanceof Chart)
			return new ChartEditForm((Chart)object, manager);
		else if (object instanceof ChartSheet)
			return new ChartSheetEditForm((ChartSheet)object);
		else if (object instanceof Dataset)
			return new DatasetEditForm((Dataset)object);
		else if (object instanceof InputFile)
			return new InputFileEditForm((InputFile)object);
		else if (object instanceof ProcessingOp)
			return new ProcessingOperationEditForm((ProcessingOp)object);
		else if (object instanceof SetOperation)
			return new SetOperationEditForm((SetOperation)object, manager);
		else
			return new GenericScaveObjectEditForm(object);
	}
	
	/**
	 * Creates a form containing the specified features of the object.
	 */
	public IScaveObjectEditForm createForm(EObject object, EStructuralFeature[] features) {
		return new GenericScaveObjectEditForm(object, features);
	}
}
