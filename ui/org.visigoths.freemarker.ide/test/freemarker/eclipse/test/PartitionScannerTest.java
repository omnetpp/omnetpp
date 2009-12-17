/*
 * 
 */
package freemarker.eclipse.test;

import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.Token;

import freemarker.eclipse.editors.PartitionScanner;
import freemarker.eclipse.test.util.TokenList;

/**
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: PartitionScannerTest.java,v 1.1 2004/03/03 21:11:42 stephanmueller Exp $
 */
public class PartitionScannerTest extends AbstractScannerTestCase {

	private IToken DIRECTIVE_TOKEN = new Token(PartitionScanner.FTL_DIRECTIVE);
	private IToken DEFAULT_TOKEN = new Token(null);
	private IToken COMMENT_TOKEN = new Token(PartitionScanner.FTL_COMMENT);
	
	public PartitionScannerTest(String name) {
		super(name);
	}
	
	public void setUp() {
		tokens = new TokenList();
		text = "<#if foo>bar</#if>";
		scanner = new PartitionScanner();
		tokens.addToken(DIRECTIVE_TOKEN,9);
		tokens.addToken(DEFAULT_TOKEN,3);
		tokens.addToken(DIRECTIVE_TOKEN,6);
	}

}
