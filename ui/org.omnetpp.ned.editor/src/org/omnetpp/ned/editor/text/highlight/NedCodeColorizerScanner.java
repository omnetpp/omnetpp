package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.common.editor.text.NedSyntaxHighlightHelper;

/**
 * A NED code scanner.
 *
 * @author rhornig
 */
public class NedCodeColorizerScanner extends RuleBasedScanner {
    
	/**
	 * Creates a NED code scanner
	 */
	public NedCodeColorizerScanner() {

        setDefaultReturnToken(NedSyntaxHighlightHelper.codeDefaultToken);

		List<IRule> rules= new ArrayList<IRule>();

        // Add rule for strings
		rules.add(new SingleLineRule("\"", "\"", NedSyntaxHighlightHelper.codeStringToken, '\\'));
        
        // Add rule for detecting numeric constants
        rules.add(new NumberRule(NedSyntaxHighlightHelper.codeNumberToken));

        // Add word rule for special keywords that require non-standard word detector
        WordRule wordRuleSpecial= new WordRule(new NedSyntaxHighlightHelper.NedSpecialWordDetector(), Token.UNDEFINED);
        for (int i= 0; i < NedSyntaxHighlightHelper.highlightNedSpecialKeywords.length; i++)
            wordRuleSpecial.addWord(NedSyntaxHighlightHelper.highlightNedSpecialKeywords[i], NedSyntaxHighlightHelper.codeKeywordToken);
        rules.add(wordRuleSpecial);
        
        // detect properties (in the form "@propname")
        rules.add(new WordRule(new NedSyntaxHighlightHelper.NedAtWordDetector(), NedSyntaxHighlightHelper.codePropertyToken));

		// Add word rule for keywords, types, and constants. If not recognized as either of these
        // then this is a regular identifier, so return an identifierToken by default
		WordRule wordRule= new WordRule(new NedSyntaxHighlightHelper.NedWordDetector(), NedSyntaxHighlightHelper.codeIdentifierToken);
		for (int i= 0; i < NedSyntaxHighlightHelper.highlightNedKeywords.length; i++)
			wordRule.addWord(NedSyntaxHighlightHelper.highlightNedKeywords[i], NedSyntaxHighlightHelper.codeKeywordToken);
        for (int i= 0; i < NedSyntaxHighlightHelper.highlightNedFunctions.length; i++)
            wordRule.addWord(NedSyntaxHighlightHelper.highlightNedFunctions[i], NedSyntaxHighlightHelper.codeFunctionToken);
		for (int i= 0; i < NedSyntaxHighlightHelper.highlightNedTypes.length; i++)
			wordRule.addWord(NedSyntaxHighlightHelper.highlightNedTypes[i], NedSyntaxHighlightHelper.codeTypeToken);
		for (int i= 0; i < NedSyntaxHighlightHelper.highlightConstants.length; i++)
			wordRule.addWord(NedSyntaxHighlightHelper.highlightConstants[i], NedSyntaxHighlightHelper.codeStringToken);
		rules.add(wordRule);
        
		IRule[] result= new IRule[rules.size()];
		rules.toArray(result);
		setRules(result);
	}
}
