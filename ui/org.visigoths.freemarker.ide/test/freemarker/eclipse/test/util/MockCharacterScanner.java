package freemarker.eclipse.test.util;

import org.eclipse.jface.text.rules.ICharacterScanner;

/**
 * An implementation of ICharacterScanner. AbstractRuleTestCase uses this class
 * to pass a piece of ftl code to an IRule.
 *
 * @see freemarker.eclipse.test.AbstractRuleTestCase
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: MockCharacterScanner.java,v 1.1 2004/02/05 00:17:52 stephanmueller Exp $
 */
public class MockCharacterScanner implements ICharacterScanner {

	private int pos = 0;
	private String text = "";

	public MockCharacterScanner(String text) {
		this.text = text;
	}

	public int getColumn() {
		throw new UnsupportedOperationException();
	}

	public char[][] getLegalLineDelimiters() {
		return new char[][] { "\n".toCharArray()};
	}

	public int read() {
		return text.charAt(pos++);
	}

	public void unread() {
		pos--;
	}

	public int getPos() {
		return pos;
	}
}