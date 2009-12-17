package freemarker.eclipse.test;

import freemarker.eclipse.editors.DirectiveRule;

/**
 * Tests {@link freemarker.eclipse.editors.DirectiveRule}.
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: DirectiveRuleTest.java,v 1.1 2004/02/05 00:17:52 stephanmueller Exp $
 */
public class DirectiveRuleTest extends AbstractRuleTestCase {

	public DirectiveRuleTest(String name) {
		super(name);
	}
	
	public void setUp() {
		super.addNonMatchingTest("foobar","Testing no-matching token");
		super.addMatchingTest("<#assign a=b />foobar",15,"Testing simple directive");
		rule = new DirectiveRule(MATCH);
	}

}
