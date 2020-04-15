/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.NumberRule;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.rules.SingleLineRule;
import org.eclipse.jface.text.rules.WordRule;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.text.InifileTextEditorHelper;

/**
 * An inifile code scanner.
 */
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
        for (String text : SyntaxHighlightHelper.highlightNedFunctions)
            wordRule.addWord(text, InifileTextEditorHelper.codeFunctionToken);
        for (String text : SyntaxHighlightHelper.highlightNedKeywords)
            wordRule.addWord(text, InifileTextEditorHelper.codeKeywordToken);
        for (String text : SyntaxHighlightHelper.highlightConstants)
            wordRule.addWord(text, InifileTextEditorHelper.codeKeywordToken);
        for (ConfigOption entry : ConfigRegistry.getOptions())
            wordRule.addWord(entry.getName(), InifileTextEditorHelper.codeConfigKeyToken);
        for (ConfigOption entry : ConfigRegistry.getPerObjectOptions())
            wordRule.addWord(entry.getName(), InifileTextEditorHelper.codeConfigKeyToken);

        rules.add(wordRule);

        setRules(rules.toArray(new IRule[]{}));
    }
}
