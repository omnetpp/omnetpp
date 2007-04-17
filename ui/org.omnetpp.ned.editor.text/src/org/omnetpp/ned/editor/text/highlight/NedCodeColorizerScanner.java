package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.common.editor.text.NedTextEditorHelper;

/**
 * A NED code scanner.
 */
public class NedCodeColorizerScanner extends RuleBasedScanner {

    
	/**
	 * Creates a NED code scanner
	 */
	public NedCodeColorizerScanner() {

        setDefaultReturnToken(NedTextEditorHelper.codeDefaultToken);

		List<IRule> rules= new ArrayList<IRule>();

        // Add rule for strings
		rules.add(new SingleLineRule("\"", "\"", NedTextEditorHelper.codeStringToken, '\\'));
        
        // Add rule for detecting numeric constants
        rules.add(new NumberRule(NedTextEditorHelper.codeNumberToken));

        // Add word rule for special keywords that require non-standard word detector
        WordRule wordRuleSpecial= new WordRule(NedTextEditorHelper.nedSpecialWordDetector, Token.UNDEFINED);
        for (int i= 0; i < NedTextEditorHelper.highlightNedSpecialKeywords.length; i++)
            wordRuleSpecial.addWord(NedTextEditorHelper.highlightNedSpecialKeywords[i], NedTextEditorHelper.codeKeywordToken);
        rules.add(wordRuleSpecial);
        
        // detect properties (in form of @propname)
        rules.add(new WordRule(NedTextEditorHelper.nedAtWordDetector, NedTextEditorHelper.codePropertyToken));

		// Add word rule for keywords, types, and constants. If not recognized as either of these
        // then this is a regular identifier, so return an identifierToken by default
		WordRule wordRule= new WordRule(NedTextEditorHelper.nedWordDetector, NedTextEditorHelper.codeIdentifierToken);
		for (int i= 0; i < NedTextEditorHelper.highlightNedKeywords.length; i++)
			wordRule.addWord(NedTextEditorHelper.highlightNedKeywords[i], NedTextEditorHelper.codeKeywordToken);
        for (int i= 0; i < NedTextEditorHelper.highlightNedFunctions.length; i++)
            wordRule.addWord(NedTextEditorHelper.highlightNedFunctions[i], NedTextEditorHelper.codeFunctionToken);
		for (int i= 0; i < NedTextEditorHelper.highlightNedTypes.length; i++)
			wordRule.addWord(NedTextEditorHelper.highlightNedTypes[i], NedTextEditorHelper.codeTypeToken);
		for (int i= 0; i < NedTextEditorHelper.highlightConstants.length; i++)
			wordRule.addWord(NedTextEditorHelper.highlightConstants[i], NedTextEditorHelper.codeStringToken);
		rules.add(wordRule);
        
		IRule[] result= new IRule[rules.size()];
		rules.toArray(result);
		setRules(result);
	}
}
