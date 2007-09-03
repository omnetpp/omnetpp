package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.common.editor.text.NedSyntaxHighlightHelper;

/**
 * A rule based NedDoc scanner.
 *
 * @author rhornig
 */
public class NedDocColorizerScanner extends RuleBasedScanner {


    /**
	 * Create a new neddoc scanner for the given color provider.
	 * 
	 * @param provider the color provider
	 */
	 public NedDocColorizerScanner() {
		super();
        // this is the default token for a comment
        setDefaultReturnToken(NedSyntaxHighlightHelper.docDefaultToken);

		List<IRule> list= new ArrayList<IRule>();

        // Add word rule for supported HTML tags
        // FIXME does not match if token is followed immediately with some characters
        WordRule tagRule= new WordRule(new NedSyntaxHighlightHelper.NedDocTagDetector(), Token.UNDEFINED);
        for (int i= 0; i < NedSyntaxHighlightHelper.highlightDocTags.length; i++)
        {
            String cTag = NedSyntaxHighlightHelper.highlightDocTags[i];
            list.add(new SingleLineRule("<" + cTag + " ", ">", NedSyntaxHighlightHelper.docTagToken, (char)0, true));   // start tag with attributes
            tagRule.addWord("<" + cTag + ">", NedSyntaxHighlightHelper.docTagToken);    // empty start tag
            tagRule.addWord("</" + cTag + ">", NedSyntaxHighlightHelper.docTagToken);   // empty end tag
            tagRule.addWord("<" + cTag + "/>", NedSyntaxHighlightHelper.docTagToken);   // combined start / end tag
        }
        list.add(tagRule);

        // Add word rule for keywords.
        WordRule keywordRule= new WordRule(new NedSyntaxHighlightHelper.NedAtWordDetector(), Token.UNDEFINED);
        for (int i= 0; i < NedSyntaxHighlightHelper.highlightDocKeywords.length; i++)
            keywordRule.addWord("@" + NedSyntaxHighlightHelper.highlightDocKeywords[i], NedSyntaxHighlightHelper.docKeywordToken);
        list.add(keywordRule);
        
		IRule[] result= new IRule[list.size()];
		list.toArray(result);
		setRules(result);
	}
}
