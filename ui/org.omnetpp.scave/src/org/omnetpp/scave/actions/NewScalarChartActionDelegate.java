package org.omnetpp.scave.actions;

/**
 * Opens a new dataset window
 */
public class NewScalarChartActionDelegate extends NewDocActionDelegate {

	private static int counter = 0;

	protected String getNewFileName() {
		return "Untitled scalar chart "+(++counter)+".schart";
	}
}
