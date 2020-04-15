/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.NumberRule;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.rules.SingleLineRule;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.jface.text.rules.WordRule;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper;

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

        setDefaultReturnToken(SyntaxHighlightHelper.codeDefaultToken);

        List<IRule> rules= new ArrayList<IRule>();

        // Add rule for strings
        rules.add(new SingleLineRule("\"", "\"", SyntaxHighlightHelper.codeStringToken, '\\'));

        // Add rule for detecting numeric constants
        rules.add(new NumberRule(SyntaxHighlightHelper.codeNumberToken));

        // Add word rule for special keywords that require non-standard word detector
        WordRule wordRuleSpecial= new WordRule(new SyntaxHighlightHelper.NedSpecialWordDetector(), Token.UNDEFINED);
        for (int i= 0; i < SyntaxHighlightHelper.highlightNedSpecialKeywords.length; i++)
            wordRuleSpecial.addWord(SyntaxHighlightHelper.highlightNedSpecialKeywords[i], SyntaxHighlightHelper.codeKeywordToken);
        rules.add(wordRuleSpecial);

        // detect properties (in the form "@propname")
        rules.add(new WordRule(new SyntaxHighlightHelper.NedAtWordDetector(), SyntaxHighlightHelper.codePropertyToken));

        // Add word rule for keywords, types, and constants. If not recognized as either of these
        // then this is a regular identifier, so return an identifierToken by default
        WordRule wordRule= new WordRule(new SyntaxHighlightHelper.NedWordDetector(), SyntaxHighlightHelper.codeIdentifierToken);
        for (int i= 0; i < SyntaxHighlightHelper.highlightNedKeywords.length; i++)
            wordRule.addWord(SyntaxHighlightHelper.highlightNedKeywords[i], SyntaxHighlightHelper.codeKeywordToken);
        for (int i= 0; i < SyntaxHighlightHelper.highlightNedFunctions.length; i++)
            wordRule.addWord(SyntaxHighlightHelper.highlightNedFunctions[i], SyntaxHighlightHelper.codeFunctionToken);
        for (int i= 0; i < SyntaxHighlightHelper.highlightNedTypes.length; i++)
            wordRule.addWord(SyntaxHighlightHelper.highlightNedTypes[i], SyntaxHighlightHelper.codeTypeToken);
        for (int i= 0; i < SyntaxHighlightHelper.highlightConstants.length; i++)
            wordRule.addWord(SyntaxHighlightHelper.highlightConstants[i], SyntaxHighlightHelper.codeStringToken);
        rules.add(wordRule);

        IRule[] result= new IRule[rules.size()];
        rules.toArray(result);
        setRules(result);
    }
}
