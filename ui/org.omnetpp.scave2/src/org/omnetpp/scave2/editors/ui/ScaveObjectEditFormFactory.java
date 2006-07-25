package org.omnetpp.scave2.editors.ui;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;

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
	 */
	public IScaveObjectEditForm createForm(EObject object) {
		return new GenericScaveObjectEditForm(object);
	}
	
	/**
	 * Creates a form containing the specified features of the object.
	 */
	public IScaveObjectEditForm createForm(EObject object, EStructuralFeature[] features) {
		return new GenericScaveObjectEditForm(object, features);
	}
}
