package org.omnetpp.scave.charting;

import java.math.BigDecimal;

/**
 * Interface for enumerating the ticks.
 *
 * @author tomi
 */
public interface ITicks extends Iterable<BigDecimal> {
	
	boolean isMajorTick(BigDecimal d);
}
