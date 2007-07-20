package org.omnetpp.common.canvas;

import java.util.NoSuchElementException;
import java.util.Scanner;

/**
 * Rectangular area with double precision coordinates.
 * Used by {@link ZoomableCachingCanvas}.
 *
 * @author tomi
 */
public class RectangularArea {
	public double minX;
	public double minY;
	public double maxX;
	public double maxY;
	
	public RectangularArea() {
	}
	
	public RectangularArea(double minX, double minY, double maxX, double maxY) {
		this.minX = minX;
		this.minY = minY;
		this.maxX = maxX;
		this.maxY = maxY;
	}
	
	public String toString() {
		return String.format("Area(%s,%s,%s,%s)", minX, minY, maxX, maxY);
	}
	
	public static RectangularArea fromString(String str) {
		Scanner s = new Scanner(str);
		try {
			s.useDelimiter("[,()]");
			s.next("Area");
			RectangularArea area = new RectangularArea();
			area.minX = s.nextDouble();
			area.minY = s.nextDouble();
			area.maxX = s.nextDouble();
			area.maxY = s.nextDouble();
			return area;
		} catch (NoSuchElementException e) {
			return null;
		} finally {
			s.close();
		}
	}
}