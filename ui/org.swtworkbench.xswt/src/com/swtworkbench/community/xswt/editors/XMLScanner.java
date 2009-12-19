package com.swtworkbench.community.xswt.editors;

import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.rules.SingleLineRule;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.jface.text.rules.WhitespaceRule;

public class XMLScanner extends RuleBasedScanner {
    public XMLScanner(ColorManager manager) {
        IToken procInstr = new Token(new TextAttribute(manager.getColor(IXMLColorConstants.PROC_INSTR)));

        IRule[] rules = new IRule[2];

        rules[0] = new SingleLineRule("<?", "?>", procInstr);

        rules[1] = new WhitespaceRule(new XMLWhitespaceDetector());

        setRules(rules);
    }
}