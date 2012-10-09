package freemarker.eclipse.test;

import freemarker.eclipse.editors.XmlScanner;
import freemarker.eclipse.test.util.TokenList;

/**
 * Tests {@link freemarker.eclipse.editors.XmlScanner}.
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: XmlScannerTest.java,v 1.2 2004/02/05 14:08:42 stephanmueller Exp $
 */
public class XmlScannerTest extends AbstractScannerTestCase {

    public XmlScannerTest(String name) {
        super(name);
    }

    public void setUp() {
        super.text = "<foobar ${foo}=<#-- -->\"bar\" foo=\"te${bar}<#-- bla ${foo} -->xt\" bar=\"foo\"/>";
        tokens = new TokenList();
        tokens.addToken(manager.getXmlToken(), 7);
        tokens.addToken(manager.getWhitespaceToken(), 1);
        tokens.addToken(manager.getInterpolationToken(),6);
        tokens.addToken(manager.getXmlToken(),1);
        tokens.addToken(manager.getCommentToken(),8);
        tokens.addToken(manager.getStringToken(),5);
        tokens.addToken(manager.getWhitespaceToken(), 1);
        tokens.addToken(manager.getXmlToken(),4);
        tokens.addToken(manager.getStringToken(),3);
        tokens.addToken(manager.getInterpolationToken(),6);
        tokens.addToken(manager.getCommentToken(),19);
        tokens.addToken(manager.getStringToken(),3);
        tokens.addToken(manager.getWhitespaceToken(), 1);
        tokens.addToken(manager.getXmlToken(),4);
        tokens.addToken(manager.getStringToken(),5);
        tokens.addToken(manager.getXmlToken(),2);
        super.scanner = new XmlScanner(super.manager);
        ((XmlScanner)scanner).setDefaultReturnToken(manager.getXmlToken());
    }
}
