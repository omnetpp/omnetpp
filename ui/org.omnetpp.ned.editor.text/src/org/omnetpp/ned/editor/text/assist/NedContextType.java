package org.omnetpp.ned.editor.text.assist;

import org.eclipse.jface.text.templates.TemplateContextType;
import org.eclipse.jface.text.templates.GlobalTemplateVariables;


/**
 * A very simple context type.
 */
public class NedContextType extends TemplateContextType {

	/** This context's id */
	public static final String DEFAULT_CONTEXT_TYPE= "org.omnetpp.ned.editor.text.default"; //$NON-NLS-1$

	/**
	 * Creates a new XML context type. 
	 */
	public NedContextType() {
		addGlobalResolvers();
	}

	private void addGlobalResolvers() {
		addResolver(new GlobalTemplateVariables.Cursor());
		addResolver(new GlobalTemplateVariables.WordSelection());
		addResolver(new GlobalTemplateVariables.LineSelection());
		addResolver(new GlobalTemplateVariables.Dollar());
		addResolver(new GlobalTemplateVariables.Date());
		addResolver(new GlobalTemplateVariables.Year());
		addResolver(new GlobalTemplateVariables.Time());
		addResolver(new GlobalTemplateVariables.User());
	}

}
