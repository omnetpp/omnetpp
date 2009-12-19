package freemarker.eclipse.test;

import junit.framework.Test;
import junit.framework.TestSuite;

/**
 * The test suite of the FM eclipse plug-in. Beside the eclipse classes and 
 * libs it requires the CGLIB2 lib for mock generation at runtime.
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: FreeMarkerPluginTestSuite.java,v 1.2 2004/03/03 21:13:32 stephanmueller Exp $
 */
public class FreeMarkerPluginTestSuite {

	public static Test suite() {
		TestSuite suite = new TestSuite();
		suite.addTest(new InterpolationRuleTest("Testing InterpolationRule"));
		suite.addTest(new DirectiveRuleTest("Testing DirectiveRule"));
		suite.addTest(new XmlScannerTest("Testing XmlScanner"));
		suite.addTest(new DirectiveScannerTest("Testing DirectiveScanner"));
		suite.addTest(new PartitionScannerTest("Testing PartitionScanner"));
		suite.setName("FreeMarkerPluginTestSuite");
		return suite;
	}

	public static void main(String[] args) {
		junit.textui.TestRunner.run(suite());
		// junit.swingui.TestRunner.run (FreeMarkerTestSuite.class);
		// junit.awtui.TestRunner.run (FreeMarkerTestSuite.class);
	}
}
