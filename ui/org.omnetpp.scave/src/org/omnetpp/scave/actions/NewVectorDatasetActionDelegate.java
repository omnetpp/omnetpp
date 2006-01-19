package org.omnetpp.scave.actions;

/**
 * Opens a new dataset window
 */
public class NewVectorDatasetActionDelegate extends NewDocActionDelegate {

	private static int counter = 0;

	protected String getNewFileName() {
		return "Untitled vector dataset "+(++counter)+".vdataset";
	}
}
