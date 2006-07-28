package org.omnetpp.scave2.editors.ui;

import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.AddDiscardOp;

/**
 * Edit form for Add/Discard operations.
 *
 * @author tomi
 */
public class AddDiscardEditForm extends SetOperationEditForm implements IScaveObjectEditForm {

	public AddDiscardEditForm(AddDiscardOp addDiscardOp) {
		super(addDiscardOp);
	}
	
	public String getTitle() {
		return setOperation instanceof Add ? "Add operation" : "Discard operation";
	}

	public String getDescription() {
		return "Modify the properties of the operation.";
	}
}
