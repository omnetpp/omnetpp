package freemarker.eclipse.test.util;

import java.util.ArrayList;

import org.eclipse.jface.text.rules.IToken;

/**
 * A list of tokens. Provides convenience method to add a token for
 * a range of indices. Used by ScannerTest-s.
 * 
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: TokenList.java,v 1.1 2004/02/05 00:17:52 stephanmueller Exp $
 */
public class TokenList extends ArrayList<IToken> {
	private static final long serialVersionUID = -1684933101782004077L;

	public void addToken(IToken token, int count) {
		for (int i = 0; i < count; i++) {
			this.add(token);
		}
	}
	
	public IToken getToken(int i) {
		return this.get(i);
	}
	
}
