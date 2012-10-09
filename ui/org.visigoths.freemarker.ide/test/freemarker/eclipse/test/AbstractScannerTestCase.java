package freemarker.eclipse.test;

import java.lang.reflect.Proxy;

import junit.framework.TestCase;

import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.ITokenScanner;
import org.eclipse.jface.text.rules.Token;

import freemarker.eclipse.editors.ITokenManager;
import freemarker.eclipse.test.util.MockDocument;
import freemarker.eclipse.test.util.MockTokenManager;
import freemarker.eclipse.test.util.TokenList;

/**
 * Superclass for all scanner tests. A subclass should provide a piece of code
 * and a TokenList which holds the sequence of tokens which should be returned
 * by the scanner under test.
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: AbstractScannerTestCase.java,v 1.2 2004/03/03 21:16:02 stephanmueller Exp $
 */
public abstract class AbstractScannerTestCase extends TestCase {

    protected TokenList tokens;
    protected String text;
    protected ITokenScanner scanner;
    protected ITokenManager manager;

    public AbstractScannerTestCase(String name) {
        super(name);
        manager = new MockTokenManager();
    }

    public void assertFit(
        String message,
        ITokenScanner scanner,
        TokenList tl) {
        IToken token = scanner.nextToken();
        int t = 0;
        while (!token.isEOF()) {
            if (token.equals(Token.WHITESPACE)) {
                token = manager.getWhitespaceToken();
            }
            for (int i = 0; i < scanner.getTokenLength(); i++) {
                String msg = "Character " + t + ", expected: " +
                       tl.getToken(t).getData()+ ", found: " + token.getData();
                assertTrue(msg, equal(token, tl.getToken(t)));
                t++;
            }
            token = scanner.nextToken();
        }
    }

    public void assertFit(ITokenScanner scanner, TokenList tl) {
        assertFit(null, scanner, tl);
    }

    private boolean equal(IToken t1, IToken t2) {
        return (
            (t1.isWhitespace() && t2.isWhitespace())
                || (t1.isUndefined() && t2.isUndefined())
                || (t1.isEOF() && t2.isEOF())
                || (t1.equals(t2))
                || (t2.getData()==null && t1.getData()==null)
                || (t2.getData().equals(t1.getData()))
                );
    }

    public void runTest() {
        IDocument document = makeMockDocument();
        document.set(text);
        ITokenScanner xs = scanner;
        xs.setRange(document, 0, text.length());
        assertFit(scanner, tokens);
    }

    public static IDocument makeMockDocument() {
        return (IDocument) Proxy.newProxyInstance(
            IDocument.class.getClassLoader(),
            new Class[] { IDocument.class },
            new MockDocument());
    }

}
