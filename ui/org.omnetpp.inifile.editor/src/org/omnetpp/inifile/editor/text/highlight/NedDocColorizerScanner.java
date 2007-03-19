package org.omnetpp.inifile.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.inifile.editor.text.NedHelper;

/**
 * A rule based NedDoc scanner.
 */
//XXX TODO rename, revise, possibly remove...
public class NedDocColorizerScanner extends RuleBasedScanner {


    /**
	 * Create a new neddoc scanner for the given color provider.
	 * 
	 * @param provider the color provider
	 */
	 public NedDocColorizerScanner() {
		super();
        // this is the default token for a comment
        setDefaultReturnToken(NedHelper.docDefaultToken);

		List<IRule> list= new ArrayList<IRule>();

		IRule[] result= new IRule[list.size()];
		list.toArray(result);
		setRules(result);
	}
}
