/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.jface.text.rules.NumberRule;
import org.eclipse.jface.text.rules.PatternRule;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.rules.SingleLineRule;
import org.eclipse.jface.text.rules.WordPatternRule;
import org.eclipse.jface.text.rules.WordRule;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper;
import org.omnetpp.common.util.StringUtils;
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
        rules.add(new SingleLineRule("\"", "\"", InifileTextEditorHelper.codeStringToken, '\\', true, true));

        // Add rule for detecting numeric constants
        rules.add(new NumberRule(InifileTextEditorHelper.codeNumberToken));

        // Add rules for options like "seed-%-mt". Needs to match "seed-17-mt", "seed-*-mt", "seed-{5..10}-mt".
        rules.add(makeRuleForVaryingConfigOption(ConfigRegistry.CFGID_SEED_n_MT));
        rules.add(makeRuleForVaryingConfigOption(ConfigRegistry.CFGID_SEED_n_LCG32));
        rules.add(makeRuleForVaryingConfigOption(ConfigRegistry.CFGID_RNG_n));

        // Add word rule for keywords, types, and constants. If not recognized as either of these
        // then this is a regular identifier, so return an identifierToken by default
        WordRule wordRule = new WordRule(InifileTextEditorHelper.inifileWordDetector, InifileTextEditorHelper.codeIdentifierToken);
        for (String text : SyntaxHighlightHelper.highlightNedFunctions)
            wordRule.addWord(text, InifileTextEditorHelper.codeFunctionToken);
        for (String text : SyntaxHighlightHelper.highlightNedExprKeywords)
            wordRule.addWord(text, InifileTextEditorHelper.codeKeywordToken);
        for (String text : SyntaxHighlightHelper.highlightConstants)
            wordRule.addWord(text, InifileTextEditorHelper.codeBoolToken);
        for (ConfigOption entry : ConfigRegistry.getOptions())
            if (!entry.getName().contains("%"))
                wordRule.addWord(entry.getName(), InifileTextEditorHelper.codeConfigKeyToken);
        for (ConfigOption entry : ConfigRegistry.getPerObjectOptions())
            wordRule.addWord(entry.getName(), InifileTextEditorHelper.codeConfigKeyToken);
        rules.add(wordRule);

        PatternRule sectionHeadingRule = new PatternRule("[", "]", InifileTextEditorHelper.sectionHeadingToken, '\\', true, true, true);
        sectionHeadingRule.setColumnConstraint(0);
        rules.add(sectionHeadingRule);

        setRules(rules.toArray(new IRule[]{}));
    }

    static class ConfigKeyWithWildcardWordDetector implements IWordDetector {
        public boolean isWordStart(char character) {
            return Character.isLetter(character) || character == '_';
        }

        public boolean isWordPart(char character) {
            return Character.isLetterOrDigit(character) || "_-*{}.".indexOf(character) != -1;
        }
    }

    private static PatternRule makeRuleForVaryingConfigOption(ConfigOption option) {
        String name = option.getName();
        String startSeq = StringUtils.substringBefore(name, "%");
        String endSeq = StringUtils.substringAfter(name, "%");
        return new WordPatternRule(new ConfigKeyWithWildcardWordDetector(), startSeq, endSeq, InifileTextEditorHelper.codeConfigKeyToken);
    }
}
