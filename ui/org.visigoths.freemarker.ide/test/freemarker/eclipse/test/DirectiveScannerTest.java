
package freemarker.eclipse.test;

import freemarker.eclipse.editors.DirectiveScanner;
import freemarker.eclipse.test.util.TokenList;

/**
 * Tests {@link freemarker.eclipse.editors.DirectiveScanner}.
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: DirectiveScannerTest.java,v 1.2 2004/02/05 14:08:42 stephanmueller Exp $
 */
public class DirectiveScannerTest extends AbstractScannerTestCase {

	public DirectiveScannerTest(String name) {
		super(name);
	}
	
	public void setUp() {
		DirectiveScanner ds = new DirectiveScanner(manager);
		ds.setDefaultReturnToken(manager.getDirectiveToken());
		scanner = ds;
		text = "<#if \"abc\"!=\"de${fo}f\" <#-- foobar --> >";
		tokens = new TokenList();
		tokens.addToken(manager.getDirectiveToken(),4);
		tokens.addToken(manager.getWhitespaceToken(),1);
		tokens.addToken(manager.getStringToken(),5);
		tokens.addToken(manager.getDirectiveToken(),2);
		tokens.addToken(manager.getStringToken(),3);
		
		// TODO: Interpolations within strings fail ATM :-(
		tokens.addToken(manager.getInterpolationToken(),5);
		
		tokens.addToken(manager.getStringToken(),2);
		tokens.addToken(manager.getWhitespaceToken(),1);
		tokens.addToken(manager.getCommentToken(),15);
		tokens.addToken(manager.getWhitespaceToken(),1);
		tokens.addToken(manager.getDirectiveToken(),1);
	}

}
