package org.omnetpp.scave.charting.dataset;

/**
 * Applies a transformation to dataset coordinates.
 * Transformations have to preserve ordering of the coordinates.
 * 
 * @author tomi
 */
public interface IDatasetTransform {
	
	double transformX(double x);
	
	double transformY(double y);
	
	double inverseTransformX(double x);
	
	double inverseTransformY(double y);
}
