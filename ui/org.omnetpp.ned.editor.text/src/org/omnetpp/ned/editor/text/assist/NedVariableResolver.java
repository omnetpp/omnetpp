package org.omnetpp.ned.editor.text.assist;

import java.util.Arrays;
import java.util.Comparator;

import org.eclipse.jface.text.templates.TemplateContext;
import org.eclipse.jface.text.templates.TemplateVariableResolver;

/**
 * Looks up existing ant variables and proposes them. The proposals are sorted by 
 * their prefix-likeness with the variable type.
 */
// TODO several variable resolver should be implemented, like nedfile chooser, icon chooser, module type and local parameter chooser
public class NedVariableResolver extends TemplateVariableResolver {
	/*
	 * @see org.eclipse.jface.text.templates.TemplateVariableResolver#resolveAll(org.eclipse.jface.text.templates.TemplateContext)
	 */
	protected String[] resolveAll(TemplateContext context) {
		String[] proposals= new String[] { "${srcDir}", "${dstDir}" }; //$NON-NLS-1$ //$NON-NLS-2$
		
		Arrays.sort(proposals, new Comparator() {

			public int compare(Object o1, Object o2) {
				return getCommonPrefixLength(getType(), (String) o2) - getCommonPrefixLength(getType(), (String) o1);
			}

			private int getCommonPrefixLength(String type, String var) {
				int i= 0;
				CharSequence vSeq= var.subSequence(2, var.length() - 1); // strip away ${}
				while (i < type.length() && i < vSeq.length())
					if (Character.toLowerCase(type.charAt(i)) == Character.toLowerCase(vSeq.charAt(i)))
						i++;
					else
						break;
				return i;
			}
		});
		
		return proposals;
	}
}
