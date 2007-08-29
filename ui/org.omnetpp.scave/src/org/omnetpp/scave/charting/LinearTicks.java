package org.omnetpp.scave.charting;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.Iterator;
import java.util.NoSuchElementException;

/**
 * Utility class for calculating ticks.
 * 
 * @author Andras
 */
public class LinearTicks implements ITicks {
	
	private BigDecimal start;
	private BigDecimal end;
	private BigDecimal majorTickDelta;
	private BigDecimal delta;
	
	public LinearTicks(double start, double end, double approxDelta) {
		int scale = (int)Math.ceil(Math.log10(approxDelta));
		this.start = new BigDecimal(start).setScale(-scale, RoundingMode.FLOOR);
		this.end = new BigDecimal(end).setScale(-scale, RoundingMode.CEILING);
		BigDecimal spacing = BigDecimal.valueOf(approxDelta);
		BigDecimal delta = new BigDecimal(1).scaleByPowerOfTen(scale);
		
		if (delta.divide(BigDecimal.valueOf(5)).compareTo(spacing) > 0) {
			// use 2, 4, 6, 8, etc. if possible
			this.majorTickDelta = delta.divide(BigDecimal.valueOf(5));
			this.delta = delta.divide(BigDecimal.TEN);
		}
		else if (delta.divide(BigDecimal.valueOf(2)).compareTo(spacing) > 0) {
			// use 5, 10, 15, 20, etc. if possible
			this.majorTickDelta = delta.divide(BigDecimal.valueOf(2));
			this.delta = delta.divide(BigDecimal.TEN);
		}
		else {
			this.majorTickDelta = delta;
			this.delta = delta.divide(BigDecimal.valueOf(5));
		}
	}

	public boolean isMajorTick(BigDecimal d) {
		return d.remainder(majorTickDelta).compareTo(BigDecimal.ZERO) == 0;
	}
	
	public Iterator<BigDecimal> iterator() {
		class TickIterator implements Iterator<BigDecimal> {
			BigDecimal current;
			
			public TickIterator() {
				current = start;
			}
			
			public boolean hasNext() {
				return current.compareTo(end) < 0;
			}

			public BigDecimal next() {
				if (!hasNext())
					throw new NoSuchElementException();
				BigDecimal result = current;
				current = current.add(delta);
				return result;
			}

			public void remove() {
				throw new UnsupportedOperationException();
			}
		};
		
		return new TickIterator();
	}
}
