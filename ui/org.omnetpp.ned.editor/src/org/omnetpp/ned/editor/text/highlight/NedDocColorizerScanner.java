/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.rules.SingleLineRule;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.jface.text.rules.WordRule;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper;

/**
 * A rule based NedDoc scanner.
 *
 * @author rhornig
 */
public class NedDocColorizerScanner extends RuleBasedScanner {


    /**
	 * Create a new neddoc scanner for the given color provider.
	 */
	 public NedDocColorizerScanner() {
		super();
        // this is the default token for a comment
        setDefaultReturnToken(SyntaxHighlightHelper.docDefaultToken);

		List<IRule> list = new ArrayList<IRule>();

        // Add single line rules for supported HTML tags
        for (int i = 0; i < SyntaxHighlightHelper.highlightDocTags.length; i++)
        {
            String cTag = SyntaxHighlightHelper.highlightDocTags[i];
            list.add(new SingleLineRule("<" + cTag + " ", ">", SyntaxHighlightHelper.docTagToken, (char)0, true));   // start tag with attributes
            list.add(new SingleLineRule("<" + cTag, ">", SyntaxHighlightHelper.docTagToken, (char)0, true));   // start tag with attributes
            list.add(new SingleLineRule("</" + cTag, ">", SyntaxHighlightHelper.docTagToken, (char)0, true));   // start tag with attributes
            list.add(new SingleLineRule("<" + cTag, "/>", SyntaxHighlightHelper.docTagToken, (char)0, true));   // start tag with attributes
        }

        // Add word rule for keywords.
        WordRule keywordRule = new WordRule(new SyntaxHighlightHelper.NedAtWordDetector(), Token.UNDEFINED);
        for (int i = 0; i < SyntaxHighlightHelper.highlightDocKeywords.length; i++)
            keywordRule.addWord("@" + SyntaxHighlightHelper.highlightDocKeywords[i], SyntaxHighlightHelper.docKeywordToken);
        list.add(keywordRule);
        
		IRule[] result = new IRule[list.size()];
		list.toArray(result);
		setRules(result);
	}
}
