package com.swtworkbench.community.xswt.editors;

import org.eclipse.jface.text.rules.ICharacterScanner;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.MultiLineRule;

public class TagRule extends MultiLineRule {
    public TagRule(IToken token) {
        super("<", ">", token);
    }

    protected boolean sequenceDetected(ICharacterScanner scanner, char[] sequence, boolean eofAllowed) {
        int c = scanner.read();
        if (sequence[0] == '<') {
            if (c == 63) {
                scanner.unread();
                return false;
            }
            if (c == 33) {
                scanner.unread();

                return false;
            }
        }
        else if (sequence[0] == '>') {
            scanner.unread();
        }
        return super.sequenceDetected(scanner, sequence, eofAllowed);
    }
}