package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.ned.editor.text.NedHelper;

/**
 * A rule based NedDoc scanner.
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
        setDefaultReturnToken(NedHelper.docDefaultToken);

		List<IRule> list= new ArrayList<IRule>();

        // Add word rule for supported HTML tags
        // FIXME does not match if token is followed immediately with some characters
        WordRule tagRule= new WordRule(NedHelper.nedDocTagDetector, Token.UNDEFINED);
        for (int i= 0; i < NedHelper.highlightDocTags.length; i++)
        {
            String cTag = NedHelper.highlightDocTags[i];
            list.add(new SingleLineRule("<" + cTag + " ", ">", NedHelper.docTagToken, (char)0, true));   // start tag with attributes
            tagRule.addWord("<" + cTag + ">", NedHelper.docTagToken);    // empty start tag
            tagRule.addWord("</" + cTag + ">", NedHelper.docTagToken);   // empty end tag
            tagRule.addWord("<" + cTag + "/>", NedHelper.docTagToken);   // combined start / end tag
        }
        list.add(tagRule);

        // Add word rule for keywords.
        WordRule keywordRule= new WordRule(NedHelper.nedAtWordDetector, Token.UNDEFINED);
        for (int i= 0; i < NedHelper.highlightDocKeywords.length; i++)
            keywordRule.addWord("@" + NedHelper.highlightDocKeywords[i], NedHelper.docKeywordToken);
        list.add(keywordRule);
        
		IRule[] result= new IRule[list.size()];
		list.toArray(result);
		setRules(result);
	}
}
