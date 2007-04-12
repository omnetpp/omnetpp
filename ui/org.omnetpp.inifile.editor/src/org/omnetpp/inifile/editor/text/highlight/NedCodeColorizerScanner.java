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

		// Add word rule for keywords, types, and constants. If not recognized as either of these
        // then this is a regular identifier, so return an identifierToken by default
		WordRule wordRule= new WordRule(NedHelper.inifileWordDetector, NedHelper.codeIdentifierToken);
        for (int i= 0; i < NedHelper.highlightNedFunctions.length; i++)
            wordRule.addWord(NedHelper.highlightNedFunctions[i], NedHelper.codeFunctionToken);
		for (int i= 0; i < NedHelper.highlightConstants.length; i++)
			wordRule.addWord(NedHelper.highlightConstants[i], NedHelper.codeStringToken);

		//XXX wordRule.addWord("sim-time-limit", NedHelper.codeKeywordToken);

		rules.add(wordRule);
        
		IRule[] result= new IRule[rules.size()];
		rules.toArray(result);
		setRules(result);
	}
}
