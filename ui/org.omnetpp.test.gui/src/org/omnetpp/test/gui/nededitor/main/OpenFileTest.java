package org.omnetpp.test.gui.nededitor.main;

import com.simulcraft.test.gui.util.WorkbenchUtils;

import org.omnetpp.test.gui.nededitor.NedFileTestCase;


public class OpenFileTest 
	extends NedFileTestCase
{
	public void testOpenFile() throws Throwable {
		createFileWithContent("simple Test {}");
		openFileFromProjectExplorerView();
		assertBothEditorsAreAccessible();
		WorkbenchUtils.assertNoErrorMessageInProblemsView();
	}
	
	public void testOpenFileWithSyntaxError() throws Throwable  {
		createFileWithContent("syntax error Test {}");
		openFileFromProjectExplorerView();
		assertBothEditorsAreAccessible();
		WorkbenchUtils.assertErrorMessageInProblemsView(".*syntax error.*");
	}

	public void testOpenFileWithTypeError() throws Throwable  {
		createFileWithContent("simple Sub extends Super {}");
		openFileFromProjectExplorerView();
		assertBothEditorsAreAccessible();
		WorkbenchUtils.assertErrorMessageInProblemsView(".*no such component.*");
	}
}
