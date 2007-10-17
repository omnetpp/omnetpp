package org.omnetpp.msg.editor.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper;

/**
 * A rule based NedDoc / MsgDoc scanner.
 *
 * @author rhornig
 */
public class MsgDocColorizerScanner extends RuleBasedScanner {


    /**
	 * Create a new neddoc scanner for the given color provider.
	 * 
	 * @param provider the color provider
	 */
	 public MsgDocColorizerScanner() {
		super();
        // this is the default token for a comment
        setDefaultReturnToken(SyntaxHighlightHelper.docDefaultToken);

		List<IRule> list= new ArrayList<IRule>();

        // Add word rule for supported HTML tags
        // FIXME does not match if token is followed immediately with some characters
        WordRule tagRule= new WordRule(new SyntaxHighlightHelper.NedDocTagDetector(), Token.UNDEFINED);
        for (int i= 0; i < SyntaxHighlightHelper.highlightDocTags.length; i++)
        {
            String cTag = SyntaxHighlightHelper.highlightDocTags[i];
            list.add(new SingleLineRule("<" + cTag + " ", ">", SyntaxHighlightHelper.docTagToken, (char)0, true));   // start tag with attributes
            tagRule.addWord("<" + cTag + ">", SyntaxHighlightHelper.docTagToken);    // empty start tag
            tagRule.addWord("</" + cTag + ">", SyntaxHighlightHelper.docTagToken);   // empty end tag
            tagRule.addWord("<" + cTag + "/>", SyntaxHighlightHelper.docTagToken);   // combined start / end tag
        }
        list.add(tagRule);

        // Add word rule for keywords.
        WordRule keywordRule= new WordRule(new SyntaxHighlightHelper.NedAtWordDetector(), Token.UNDEFINED);
        for (int i= 0; i < SyntaxHighlightHelper.highlightDocKeywords.length; i++)
            keywordRule.addWord("@" + SyntaxHighlightHelper.highlightDocKeywords[i], SyntaxHighlightHelper.docKeywordToken);
        list.add(keywordRule);
        
		IRule[] result= new IRule[list.size()];
		list.toArray(result);
		setRules(result);
	}
}
