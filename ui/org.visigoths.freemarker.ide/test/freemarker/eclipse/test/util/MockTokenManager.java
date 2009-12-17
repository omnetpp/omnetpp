package freemarker.eclipse.test.util;

import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.Token;

import freemarker.eclipse.editors.ITokenManager;

/**
 * An implementation of ITokenManager. This class is designed for unit tests,
 * where it's only relevant to distinguish between the different tokens and
 * colors etc. are of no significance.
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: MockTokenManager.java,v 1.1 2004/02/05 00:17:52 stephanmueller Exp $
 */
public class MockTokenManager implements ITokenManager {

	public static final IToken DIRECTIVE_TOKEN = new Token("DIRECTIVE");
	public static final IToken COMMENT_TOKEN = new Token("COMMENT");
	public static final IToken INTERPOLATION_TOKEN = new Token("INTEROLATION");
	public static final IToken XML_TOKEN = new Token("XML");
	public static final IToken XML_COMMENT_TOKEN = new Token("XML_COMMENT");
	public static final IToken STRING_TOKEN = new Token("STRING");
	public static final IToken TEXT_TOKEN = new Token("TEXT");
	public static final IToken WHITESPACE_TOKEN = new Token("WHITESPACE") {
		public boolean isWhitespace() {	return true;} 
	};
	
	
	public void dispose() {
		// noop
	}

	public IToken getCommentToken() {
		return COMMENT_TOKEN;
	}

	public IToken getDirectiveToken() {
		return DIRECTIVE_TOKEN;
	}

	public IToken getInterpolationToken() {
		return INTERPOLATION_TOKEN;
	}

	public IToken getStringToken() {
		return STRING_TOKEN;
	}

	public IToken getTextToken() {
		return TEXT_TOKEN;
	}

	public IToken getXmlCommentToken() {
		return XML_COMMENT_TOKEN;
	}

	public IToken getXmlToken() {
		return XML_TOKEN;
	}

	public IToken getWhitespaceToken() {
		return WHITESPACE_TOKEN;
	}
}
