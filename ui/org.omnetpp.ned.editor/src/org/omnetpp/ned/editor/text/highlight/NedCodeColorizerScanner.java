package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.common.editor.text.NedCompletionHelper;

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

        setDefaultReturnToken(NedCompletionHelper.codeDefaultToken);

		List<IRule> rules= new ArrayList<IRule>();

        // Add rule for strings
		rules.add(new SingleLineRule("\"", "\"", NedCompletionHelper.codeStringToken, '\\'));
        
        // Add rule for detecting numeric constants
        rules.add(new NumberRule(NedCompletionHelper.codeNumberToken));

        // Add word rule for special keywords that require non-standard word detector
        WordRule wordRuleSpecial= new WordRule(NedCompletionHelper.nedSpecialWordDetector, Token.UNDEFINED);
        for (int i= 0; i < NedCompletionHelper.highlightNedSpecialKeywords.length; i++)
            wordRuleSpecial.addWord(NedCompletionHelper.highlightNedSpecialKeywords[i], NedCompletionHelper.codeKeywordToken);
        rules.add(wordRuleSpecial);
        
        // detect properties (in form of @propname)
        rules.add(new WordRule(NedCompletionHelper.nedAtWordDetector, NedCompletionHelper.codePropertyToken));

		// Add word rule for keywords, types, and constants. If not recognized as either of these
        // then this is a regular identifier, so return an identifierToken by default
		WordRule wordRule= new WordRule(NedCompletionHelper.nedWordDetector, NedCompletionHelper.codeIdentifierToken);
		for (int i= 0; i < NedCompletionHelper.highlightNedKeywords.length; i++)
			wordRule.addWord(NedCompletionHelper.highlightNedKeywords[i], NedCompletionHelper.codeKeywordToken);
        for (int i= 0; i < NedCompletionHelper.highlightNedFunctions.length; i++)
            wordRule.addWord(NedCompletionHelper.highlightNedFunctions[i], NedCompletionHelper.codeFunctionToken);
		for (int i= 0; i < NedCompletionHelper.highlightNedTypes.length; i++)
			wordRule.addWord(NedCompletionHelper.highlightNedTypes[i], NedCompletionHelper.codeTypeToken);
		for (int i= 0; i < NedCompletionHelper.highlightConstants.length; i++)
			wordRule.addWord(NedCompletionHelper.highlightConstants[i], NedCompletionHelper.codeStringToken);
		rules.add(wordRule);
        
		IRule[] result= new IRule[rules.size()];
		rules.toArray(result);
		setRules(result);
	}
}
