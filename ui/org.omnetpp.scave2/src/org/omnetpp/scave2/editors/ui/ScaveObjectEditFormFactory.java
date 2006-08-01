package org.omnetpp.scave2.editors.ui;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.omnetpp.scave.model.AddDiscardOp;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ProcessingOp;

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
	public IScaveObjectEditForm createForm(EObject object) {
		
		if (object instanceof AddDiscardOp)
			return new AddDiscardEditForm((AddDiscardOp)object);
		if (object instanceof Chart)
			return new ChartEditForm((Chart)object);
		else if (object instanceof ChartSheet)
			return new ChartSheetEditForm((ChartSheet)object);
		else if (object instanceof Dataset)
			return new DatasetEditForm((Dataset)object);
		else if (object instanceof ProcessingOp)
			return new ProcessingOperationEditForm((ProcessingOp)object);
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
