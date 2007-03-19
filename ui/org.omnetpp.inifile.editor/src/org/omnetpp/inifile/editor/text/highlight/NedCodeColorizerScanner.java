package org.omnetpp.inifile.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.inifile.editor.text.NedHelper;

/**
 * A NED code scanner.
 */
//XXX TODO rename, revise, possibly remove...
public class NedCodeColorizerScanner extends RuleBasedScanner {

    
	/**
	 * Creates a NED code scanner
	 */
	public NedCodeColorizerScanner() {

        setDefaultReturnToken(NedHelper.codeDefaultToken);

		List<IRule> rules= new ArrayList<IRule>();

        // Add rule for strings
		rules.add(new SingleLineRule("\"", "\"", NedHelper.codeStringToken, '\\'));
        
        // Add rule for detecting numeric constants
        rules.add(new NumberRule(NedHelper.codeNumberToken));

        // Add word rule for special keywords that require non-standard word detector
        WordRule wordRuleSpecial= new WordRule(NedHelper.nedSpecialWordDetector, Token.UNDEFINED);
        for (int i= 0; i < NedHelper.highlightNedSpecialKeywords.length; i++)
            wordRuleSpecial.addWord(NedHelper.highlightNedSpecialKeywords[i], NedHelper.codeKeywordToken);
        rules.add(wordRuleSpecial);
        
        // detect properties (in form of @propname)
        rules.add(new WordRule(NedHelper.nedAtWordDetector, NedHelper.codePropertyToken));

		// Add word rule for keywords, types, and constants. If not recognized as either of these
        // then this is a regular identifier, so return an identifierToken by default
		WordRule wordRule= new WordRule(NedHelper.nedWordDetector, NedHelper.codeIdentifierToken);
		for (int i= 0; i < NedHelper.highlightNedKeywords.length; i++)
			wordRule.addWord(NedHelper.highlightNedKeywords[i], NedHelper.codeKeywordToken);
        for (int i= 0; i < NedHelper.highlightNedFunctions.length; i++)
            wordRule.addWord(NedHelper.highlightNedFunctions[i], NedHelper.codeFunctionToken);
		for (int i= 0; i < NedHelper.highlightNedTypes.length; i++)
			wordRule.addWord(NedHelper.highlightNedTypes[i], NedHelper.codeTypeToken);
		for (int i= 0; i < NedHelper.highlightConstants.length; i++)
			wordRule.addWord(NedHelper.highlightConstants[i], NedHelper.codeStringToken);
		rules.add(wordRule);
        
		IRule[] result= new IRule[rules.size()];
		rules.toArray(result);
		setRules(result);
	}
}
