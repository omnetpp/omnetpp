package freemarker.eclipse.editors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.rules.SingleLineRule;
import org.eclipse.jface.text.rules.WhitespaceRule;


/**
 * @version $Id: DirectiveScanner.java,v 1.6 2004/02/05 00:16:23 stephanmueller Exp $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 */
public class DirectiveScanner extends RuleBasedScanner {

    public DirectiveScanner(ITokenManager manager) {

        IToken string = manager.getStringToken();
        IToken comment = manager.getCommentToken();

        List<IRule> rules = new ArrayList<IRule>();

        // Add rule for double quotes
        rules.add(new SingleLineRule("\"", "\"", string,'\\'));
        // Add rule for single quotes
        rules.add(new SingleLineRule("'", "'", string,'\\'));
        // Add rule for comments
        rules.add(new SingleLineRule("<#--", "-->", comment));

        // Add generic whitespace rule.
        rules.add(new WhitespaceRule(new WhitespaceDetector()));

        IRule[] result = new IRule[rules.size()];
        rules.toArray(result);
        setRules(result);
    }
}
