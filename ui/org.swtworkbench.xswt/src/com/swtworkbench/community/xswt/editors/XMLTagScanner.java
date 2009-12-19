package com.swtworkbench.community.xswt.editors;

import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.rules.SingleLineRule;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.jface.text.rules.WhitespaceRule;

public class XMLTagScanner extends RuleBasedScanner {
    public XMLTagScanner(ColorManager manager) {
        IToken string = new Token(new TextAttribute(manager.getColor(IXMLColorConstants.STRING)));

        IRule[] rules = new IRule[3];

        rules[0] = new SingleLineRule("\"", "\"", string, '\\');

        rules[1] = new SingleLineRule("'", "'", string, '\\');

        rules[2] = new WhitespaceRule(new XMLWhitespaceDetector());

        setRules(rules);
    }
}