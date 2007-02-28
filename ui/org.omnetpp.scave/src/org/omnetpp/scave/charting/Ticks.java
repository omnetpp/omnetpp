package org.omnetpp.scave.charting;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.Iterator;
import java.util.NoSuchElementException;

/**
 * Utility class for calculating ticks 
 * @author Andras
 */
public class Ticks implements Iterable<BigDecimal> {
	
	private BigDecimal start;
	private BigDecimal end;
	private BigDecimal majorTickDelta;
	private BigDecimal delta;
	
	public Ticks(double start, double end, double delta) {
		int scale = (int)Math.ceil(Math.log10(delta));
		this.start = new BigDecimal(start).setScale(-scale, RoundingMode.FLOOR);
		this.end = new BigDecimal(end).setScale(-scale, RoundingMode.CEILING);
		BigDecimal spacing = BigDecimal.valueOf(delta);
		this.delta = new BigDecimal(1).scaleByPowerOfTen(scale);
		//this.majorTickDelta = this.delta; XXX this does not work so well, so see below
		
		// use 2, 4, 6, 8, etc. if possible
		if (this.delta.divide(BigDecimal.valueOf(5)).compareTo(spacing) > 0)
			this.delta = this.delta.divide(BigDecimal.valueOf(5));
		// use 5, 10, 15, 20, etc. if possible
		else if (this.delta.divide(BigDecimal.valueOf(2)).compareTo(spacing) > 0)
			this.delta = this.delta.divide(BigDecimal.valueOf(2));
		
		// for now, every tick is major tick (XXX until we find out a good algorithm)
		this.majorTickDelta = this.delta;
		System.out.println("MajorDelta="+majorTickDelta+" minorDelta="+this.delta);
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
