package org.omnetpp.test.gui.nededitor;

import org.omnetpp.test.gui.core.ProjectFileTestCase;

public class NedFileTestCase
	extends ProjectFileTestCase
{
	public NedFileTestCase() {
		super("test.ned");
	}

	public void assertBothEditorsAreAccessible() {
		NedEditorUtils.assertBothEditorsAreAccessible(fileName);
	}
}
