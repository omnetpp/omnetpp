package org.omnetpp.test.gui.nededitor;

import org.omnetpp.test.gui.core.ProjectFileTestCase;

public class NedFileTestCase
	extends ProjectFileTestCase
{
	public NedFileTestCase() {
		super("test.ned");
	}

	@Override
	protected void setUp() throws Exception {
		super.setUp();
		setUpInternal();
	}
	
	@Override
	protected void tearDown() throws Exception {
		super.tearDown();
		tearDownInternal();
	}
	
	@Override
	protected void setUpInternal() throws Exception {
		super.setUpInternal();
		NedEditorUtils.createNewNedFileByWizard(projectName, fileName);
	}

	protected void assertNoErrorInNedSource(String nedSource) {
		NedEditorUtils.assertNoErrorInNedSource(fileName, nedSource);
	}

	protected void assertErrorInNedSource(String nedSource, String errorTest) {
		NedEditorUtils.assertErrorInNedSource(fileName, nedSource, errorTest);
	}
}
