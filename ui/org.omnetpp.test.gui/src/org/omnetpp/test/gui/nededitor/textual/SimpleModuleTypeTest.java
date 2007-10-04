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
		assertErrorInNedSource("simple A {} simple A {}", ".*A.*already defined.*");
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
	
	@Override
	protected void setUpInternal() throws Exception {
		super.setUpInternal();
		createEmptyFile();
        WorkbenchUtils.findInProjectExplorerView(filePath).reveal().doubleClick();
	}

	protected void assertNoErrorInNedSource(String nedSource) {
		NedEditorUtils.typeIntoTextualNedEditor(fileName, nedSource);
		WorkbenchUtils.assertNoErrorMessageInProblemsView();
	}

	protected void assertErrorInNedSource(String nedSource, String errorText) {
		NedEditorUtils.typeIntoTextualNedEditor(fileName, nedSource);
		WorkbenchUtils.assertErrorMessageInProblemsView(errorText);
		//TODO: do something in the graphical editor: check error markers are there, etc
	}
}
