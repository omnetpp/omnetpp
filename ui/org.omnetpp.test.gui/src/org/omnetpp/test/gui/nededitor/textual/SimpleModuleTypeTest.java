package org.omnetpp.test.gui.nededitor.textual;

import com.simulcraft.test.gui.util.WorkbenchUtils;

import org.omnetpp.test.gui.nededitor.NedEditorUtils;
import org.omnetpp.test.gui.nededitor.NedFileTestCase;



public class SimpleModuleTypeTest
	extends NedFileTestCase
{
	//TODO: same thing with "module" and "channel" instead of "simple" in a separate class

	public void testSimple1() throws Throwable {
		assertNoErrorInNedSource("simple A {}");
	}

	public void testDuplicate1() throws Throwable {
	    // FIXME this test fails because 2 error message is generated (1 for each line)
	    // however assertErrorMessageInProblemView requires to mach only a single line
		assertErrorInNedSource("simple A {}\nsimple A {}", ".*A.*already defined.*");
	}

	public void testExtends1() throws Throwable {
		assertNoErrorInNedSource("simple A {}\nsimple B extends A {}");
	}

	public void testExtends2() throws Throwable {
		assertErrorInNedSource("simple A extends Unknown {}", ".*no such.*Unknown.*");
	}
	
	public void testLike1() throws Throwable {
		assertErrorInNedSource("simple A like Unknown {}", ".*no such.*Unknown.*");
	}

	public void testLike2() throws Throwable {
		assertErrorInNedSource("simple A {}\nsimple B like A {}", ".*A is not an interface.*");
	}

	public void testExtendsCycle1() throws Throwable {
		assertErrorInNedSource("simple A extends A {}", ".*cycle.*");
	}
	
	public void testExtendsCycle2() throws Throwable {
		assertErrorInNedSource("simple A extends B {}\nsimple B extends A {}", ".*cycle.*");
	}

	public void testExtendsCycle3() throws Throwable {
		assertErrorInNedSource("simple A extends B {}\nsimple B extends C {}\nsimple C extends A {}", ".*cycle.*");
	}
	
	protected void assertNoErrorInNedSource(String nedSource) throws Exception {
	    createFileWithContent(nedSource);
        WorkbenchUtils.findInProjectExplorerView(filePath).reveal().doubleClick();
		WorkbenchUtils.assertNoErrorMessageInProblemsView();
	}

	protected void assertErrorInNedSource(String nedSource, String errorText) throws Exception {
        createFileWithContent(nedSource);
        WorkbenchUtils.findInProjectExplorerView(filePath).reveal().doubleClick();
		WorkbenchUtils.assertErrorMessageInProblemsView(errorText);
		// TODO do something in the graphical editor: check error markers are there, etc
		// TODO also check the ruler of the text editor
	}
}
