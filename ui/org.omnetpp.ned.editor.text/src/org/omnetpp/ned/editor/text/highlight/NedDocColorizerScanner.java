package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.common.editor.text.NedTextEditorHelper;

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
        setDefaultReturnToken(NedTextEditorHelper.docDefaultToken);

		List<IRule> list= new ArrayList<IRule>();

        // Add word rule for supported HTML tags
        // FIXME does not match if token is followed immediately with some characters
        WordRule tagRule= new WordRule(NedTextEditorHelper.nedDocTagDetector, Token.UNDEFINED);
        for (int i= 0; i < NedTextEditorHelper.highlightDocTags.length; i++)
        {
            String cTag = NedTextEditorHelper.highlightDocTags[i];
            list.add(new SingleLineRule("<" + cTag + " ", ">", NedTextEditorHelper.docTagToken, (char)0, true));   // start tag with attributes
            tagRule.addWord("<" + cTag + ">", NedTextEditorHelper.docTagToken);    // empty start tag
            tagRule.addWord("</" + cTag + ">", NedTextEditorHelper.docTagToken);   // empty end tag
            tagRule.addWord("<" + cTag + "/>", NedTextEditorHelper.docTagToken);   // combined start / end tag
        }
        list.add(tagRule);

        // Add word rule for keywords.
        WordRule keywordRule= new WordRule(NedTextEditorHelper.nedAtWordDetector, Token.UNDEFINED);
        for (int i= 0; i < NedTextEditorHelper.highlightDocKeywords.length; i++)
            keywordRule.addWord("@" + NedTextEditorHelper.highlightDocKeywords[i], NedTextEditorHelper.docKeywordToken);
        list.add(keywordRule);
        
		IRule[] result= new IRule[list.size()];
		list.toArray(result);
		setRules(result);
	}
}
