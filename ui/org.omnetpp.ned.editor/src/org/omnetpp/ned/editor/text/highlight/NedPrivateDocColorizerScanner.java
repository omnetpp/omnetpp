package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.jface.text.rules.WordRule;
import org.omnetpp.common.editor.text.NedSyntaxHighlightHelper;

/**
 * A rule based PrivatedNedDoc scanner.
 *
 * @author rhornig
 */
public class NedPrivateDocColorizerScanner extends RuleBasedScanner {

    /**
	 * Create a new neddoc scanner for the given color provider.
	 * 
	 * @param provider the color provider
	 */
	 public NedPrivateDocColorizerScanner() {
		super();
        // this is the default token for a comment
        setDefaultReturnToken(NedSyntaxHighlightHelper.docPrivateDefaultToken);

		List<IRule> list= new ArrayList<IRule>();
        
        // Add word rule for private comment hints
        WordRule hintRule= new WordRule(new NedSyntaxHighlightHelper.NedWordDetector(), Token.UNDEFINED);
        for (int i= 0; i < NedSyntaxHighlightHelper.highlightPrivateDocTodo.length; i++)
            hintRule.addWord(NedSyntaxHighlightHelper.highlightPrivateDocTodo[i], NedSyntaxHighlightHelper.docPrivateTodoToken);
        list.add(hintRule);

		IRule[] result= new IRule[list.size()];
		list.toArray(result);
		setRules(result);
	}
}
