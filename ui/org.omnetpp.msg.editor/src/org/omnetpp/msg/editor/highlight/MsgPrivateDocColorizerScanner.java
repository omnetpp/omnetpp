package org.omnetpp.msg.editor.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.jface.text.rules.WordRule;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper;

/**
 * A rule based PrivatedDoc scanner.
 *
 * @author rhornig
 */
public class MsgPrivateDocColorizerScanner extends RuleBasedScanner {

    /**
	 * Create a new doc scanner for the given color provider.
	 * 
	 * @param provider the color provider
	 */
	 public MsgPrivateDocColorizerScanner() {
		super();
        // this is the default token for a comment
        setDefaultReturnToken(SyntaxHighlightHelper.docPrivateDefaultToken);

		List<IRule> list= new ArrayList<IRule>();
        
        // Add word rule for private comment hints
        WordRule hintRule= new WordRule(new SyntaxHighlightHelper.NedWordDetector(), Token.UNDEFINED);
        for (int i= 0; i < SyntaxHighlightHelper.highlightPrivateDocTodo.length; i++)
            hintRule.addWord(SyntaxHighlightHelper.highlightPrivateDocTodo[i], SyntaxHighlightHelper.docPrivateTodoToken);
        list.add(hintRule);

		IRule[] result= new IRule[list.size()];
		list.toArray(result);
		setRules(result);
	}
}
