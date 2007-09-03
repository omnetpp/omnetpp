package org.omnetpp.inifile.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.common.editor.text.NedSyntaxHighlightHelper;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.text.InifileTextEditorHelper;

/**
 * Am inifile code scanner.
 */
//XXX revise
public class InifileCodeColorizerScanner extends RuleBasedScanner {
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
        for (String text : NedSyntaxHighlightHelper.highlightNedFunctions)
			wordRule.addWord(text, InifileTextEditorHelper.codeFunctionToken);
		for (String text : NedSyntaxHighlightHelper.highlightNedKeywords)
			wordRule.addWord(text, InifileTextEditorHelper.codeKeywordToken);
		for (String text : NedSyntaxHighlightHelper.highlightConstants)
			wordRule.addWord(text, InifileTextEditorHelper.codeKeywordToken);
		for (ConfigKey entry : ConfigRegistry.getEntries())
			wordRule.addWord(entry.getKey(), InifileTextEditorHelper.codeConfigKeyToken);

		rules.add(wordRule);
        
		setRules(rules.toArray(new IRule[]{}));
	}
}
