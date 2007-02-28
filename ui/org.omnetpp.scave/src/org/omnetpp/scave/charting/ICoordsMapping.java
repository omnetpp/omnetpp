package org.omnetpp.scave.charting;

public interface ICoordsMapping {

	public double fromCanvasX(int x);

	public double fromCanvasY(int y);

	public double fromCanvasDistX(int x);

	public double fromCanvasDistY(int y);

	public int toCanvasX(double xCoord);

	public int toCanvasY(double yCoord);

	public int toCanvasDistX(double xCoord);

	public int toCanvasDistY(double yCoord);

}