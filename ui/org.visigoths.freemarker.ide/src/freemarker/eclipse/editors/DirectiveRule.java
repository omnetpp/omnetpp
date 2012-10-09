package freemarker.eclipse.editors;

import org.eclipse.jface.text.rules.ICharacterScanner;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.PatternRule;
import org.eclipse.jface.text.rules.Token;

/**
 * A FreeMarker directive rule. This rule handles all the starting
 * sequences, as well as various constructs inside the directives.
 * In particular '>' characters inside parenthesized expressions,
 * quoted strings and FreeMarker comments are handled.
 *
 * @author  <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 * @author  <a href="mailto:per&#64;percederberg.net">Per Cederberg</a>
 * @version $Id: $
 */
public class DirectiveRule extends PatternRule {

    /**
     * Creates a ftl directive rule.
     *
     * @param token the token which will be returned on success
     */
    public DirectiveRule(IToken token) {
        super("<#", ">", token, (char) 0, false);
    }

    /* (non-Javadoc)
     * @see org.eclipse.jface.text.rules.PatternRule#doEvaluate(org.eclipse.jface.text.rules.ICharacterScanner, boolean)
     */
    protected IToken doEvaluate(ICharacterScanner scanner, boolean resume) {
        if (!resume && !FreemarkerTools.readDirectiveStart(scanner)) {
            return Token.UNDEFINED;
        }
        FreemarkerTools.readDirectiveEnd(scanner);
        return fToken;
    }

}
