package freemarker.eclipse.test;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.Token;

import freemarker.eclipse.test.util.MockCharacterScanner;

/**
 * Superclass for all rule tests. A subclass can add tests by invoking
 * {@link #addMatchingTest addMatchingTest} or
 * {@link #addNonMatchingTest addNonMatchingTest}.
 * 
 * Matching tests evaluate to a token (which is checked) and a position
 * after the analysis (which is checked as well).
 * 
 * Non-matching tests should evaluate to Token.UNDEFINED and the
 * position after the analysis should be 0.
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: AbstractRuleTestCase.java,v 1.1 2004/02/05 00:17:52 stephanmueller Exp $
 */
public abstract class AbstractRuleTestCase extends TestCase {

	protected List tests = new ArrayList();
	protected IRule rule = null;

	public static final IToken NO_MATCH = new Token(null) {
		public boolean isUndefined() {
			return (true);
		}
		public String toString() {
			return "no match";
		}
	};

	public static final IToken MATCH = new Token(null) {
		public String toString() {
			return "match";
		}
	};

	public AbstractRuleTestCase(String name) {
		super(name);
	}
	
	public void runTest() {
		if (null == rule) {
			throw new RuntimeException("rule == null");
		}
		for (int i = 0; i < tests.size(); i++) {
			RuleTestInfo info = (RuleTestInfo) tests.get(i);
			MockCharacterScanner scanner =
				new MockCharacterScanner(info.text);
			IToken result = rule.evaluate(scanner);

			// we use our own 'UNDEFINED' token which looks nicer
			if (result.equals(Token.UNDEFINED))
				result = NO_MATCH;

			assertEquals(info.message, info.expected, result);
			assertEquals(info.message, info.pos, scanner.getPos());
		}
	}

	public void addMatchingTest(String text, int pos, String message) {
		tests.add(new RuleTestInfo(text, MATCH, pos, message));
	}

	public void addNonMatchingTest(String text, String message) {
		tests.add(new RuleTestInfo(text, NO_MATCH, 0, message));
	}

	private class RuleTestInfo {

		public int pos;
		public IToken expected;
		public String text;
		public String message;

		public RuleTestInfo(String text, IToken exp, int pos, String msg) {
			this.pos = pos;
			this.expected = exp;
			this.message = msg;
			this.text = text;
		}
	}

}
