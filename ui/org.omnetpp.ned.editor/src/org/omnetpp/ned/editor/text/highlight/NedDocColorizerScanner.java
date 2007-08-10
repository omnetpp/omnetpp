package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.common.editor.text.NedCompletionHelper;

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
        setDefaultReturnToken(NedCompletionHelper.docDefaultToken);

		List<IRule> list= new ArrayList<IRule>();

        // Add word rule for supported HTML tags
        // FIXME does not match if token is followed immediately with some characters
        WordRule tagRule= new WordRule(NedCompletionHelper.nedDocTagDetector, Token.UNDEFINED);
        for (int i= 0; i < NedCompletionHelper.highlightDocTags.length; i++)
        {
            String cTag = NedCompletionHelper.highlightDocTags[i];
            list.add(new SingleLineRule("<" + cTag + " ", ">", NedCompletionHelper.docTagToken, (char)0, true));   // start tag with attributes
            tagRule.addWord("<" + cTag + ">", NedCompletionHelper.docTagToken);    // empty start tag
            tagRule.addWord("</" + cTag + ">", NedCompletionHelper.docTagToken);   // empty end tag
            tagRule.addWord("<" + cTag + "/>", NedCompletionHelper.docTagToken);   // combined start / end tag
        }
        list.add(tagRule);

        // Add word rule for keywords.
        WordRule keywordRule= new WordRule(NedCompletionHelper.nedAtWordDetector, Token.UNDEFINED);
        for (int i= 0; i < NedCompletionHelper.highlightDocKeywords.length; i++)
            keywordRule.addWord("@" + NedCompletionHelper.highlightDocKeywords[i], NedCompletionHelper.docKeywordToken);
        list.add(keywordRule);
        
		IRule[] result= new IRule[list.size()];
		list.toArray(result);
		setRules(result);
	}
}
