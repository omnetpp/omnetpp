package org.omnetpp.inifile.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.inifile.editor.text.InifileTextEditorHelper;

/**
 * Am inifile code scanner.
 */
//XXX revise
public class InifileCodeColorizerScanner extends RuleBasedScanner {
	/**
	 * Creates a NED code scanner
	 */
	public InifileCodeColorizerScanner() {

        setDefaultReturnToken(InifileTextEditorHelper.codeDefaultToken);

		List<IRule> rules = new ArrayList<IRule>();

        // Add rule for strings
		rules.add(new SingleLineRule("\"", "\"", InifileTextEditorHelper.codeStringToken, '\\'));
        
        // Add rule for detecting numeric constants
        rules.add(new NumberRule(InifileTextEditorHelper.codeNumberToken));

		// Add word rule for keywords, types, and constants. If not recognized as either of these
        // then this is a regular identifier, so return an identifierToken by default
		WordRule wordRule = new WordRule(InifileTextEditorHelper.inifileWordDetector, InifileTextEditorHelper.codeIdentifierToken);
        for (int i= 0; i < InifileTextEditorHelper.highlightNedFunctions.length; i++)
            wordRule.addWord(InifileTextEditorHelper.highlightNedFunctions[i], InifileTextEditorHelper.codeFunctionToken);
		for (int i= 0; i < InifileTextEditorHelper.highlightConstants.length; i++)
			wordRule.addWord(InifileTextEditorHelper.highlightConstants[i], InifileTextEditorHelper.codeStringToken);

		//XXX wordRule.addWord("sim-time-limit", NedHelper.codeKeywordToken);

		rules.add(wordRule);
        
		setRules(rules.toArray(new IRule[]{}));
	}
}
