package org.omnetpp.scave.actions;

/**
 * Opens a new dataset window
 */
public class NewScalarDatasetActionDelegate extends NewDocActionDelegate {

	private static int counter = 0;

	protected String getNewFileName() {
		return "Untitled scalar dataset "+(++counter)+".sdataset";
	}
}
