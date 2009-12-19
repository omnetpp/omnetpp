package freemarker.eclipse.test;

import freemarker.eclipse.editors.InterpolationRule;

/**
 * Tests {@link freemarker.eclipse.editors.InterpolationRule}.
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: InterpolationRuleTest.java,v 1.2 2004/03/03 21:12:48 stephanmueller Exp $
 */
public class InterpolationRuleTest extends AbstractRuleTestCase {

	public InterpolationRuleTest(String name) {
		super(name);
	}
	
	public void setUp() {
		super.addNonMatchingTest("foobar","Testing non-matching token");
		super.addMatchingTest("${test}foobar",7,"Testing simple interpolation");
		super.addMatchingTest("${test?default(\"{foo}\")}foobar",24,"Testing complex interpolation");
		super.addMatchingTest("${{\"foo\" : \"bar\",'bar':{'foo':1}}[key]}foobar",39,"Testing hash literal inside interpolation");
		super.rule = new InterpolationRule(MATCH);
	}
}
