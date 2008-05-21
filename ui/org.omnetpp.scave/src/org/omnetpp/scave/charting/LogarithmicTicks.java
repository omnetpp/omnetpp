package org.omnetpp.scave.charting;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.math.RoundingMode;
import java.util.Iterator;
import java.util.NoSuchElementException;

import org.eclipse.core.runtime.Assert;

/**
 * Utility class for computing ticks of a logarithmic axis.
 * <p>
 * There are ticks at ... 0.2, 0.5, 1, 2, 5, 10, ...
 * from which the ... 1, 10, ... are the major ticks.
 * 
 * @author tomi
 */
public class LogarithmicTicks implements ITicks {
	
	private static BigInteger ONE = BigInteger.ONE;
	private static BigInteger TWO = BigInteger.valueOf(2);
	private static BigInteger FIVE = BigInteger.valueOf(5);
	
	BigDecimal start;
	BigDecimal end;
	
	public LogarithmicTicks(double start, double end) {
		Assert.isLegal(start > 0.0 && end > 0.0);
		this.start = asTickValue(start, RoundingMode.FLOOR);
		this.end = asTickValue(end, RoundingMode.CEILING);
	}
	
	private BigDecimal asTickValue(double value, RoundingMode rounding) {
		int scale = -(int)Math.floor(Math.log10(value));
		BigInteger unscaledValue = new BigDecimal(value).scaleByPowerOfTen(scale).setScale(0, rounding).unscaledValue();
		
		if (rounding == RoundingMode.FLOOR) {
			switch (unscaledValue.intValue()) {
			case 0:
				//Assert.isTrue(false); break;
			case 1:
				unscaledValue = ONE; break;
			case 2: case 3: case 4:
				unscaledValue = TWO; break;
			case 5: case 6: case 7: case 8: case 9:
				unscaledValue = FIVE; break;
			}
		}
		else { // CEILING
			switch (unscaledValue.intValue()) {
			case 0:
				Assert.isTrue(false); break;
			case 1: case 2:
				unscaledValue = TWO; break;
			case 3: case 4: case 5:
				unscaledValue = FIVE; break;
			case 6: case 7: case 8: case 9:
				unscaledValue = ONE; break;
			}
		}
		return new BigDecimal(unscaledValue, scale);
	}

	public boolean isMajorTick(BigDecimal d) {
		return d.unscaledValue().intValue() == 1;
	}

	public Iterator<BigDecimal> iterator() {
		class TickIterator implements Iterator<BigDecimal> {
			
			BigDecimal current;
			
			public TickIterator() {
				current = start;
			}

			public boolean hasNext() {
				return current.compareTo(end) <= 0;
			}

			public BigDecimal next() {
				if (!hasNext())
					throw new NoSuchElementException();
				BigDecimal result = current;
				int scale = current.scale();
				switch (current.unscaledValue().intValue()) {
				case 1: current = new BigDecimal(TWO, scale); break;
				case 2: current = new BigDecimal(FIVE, scale); break;
				case 5: current = new BigDecimal(ONE, scale-1); break;
				default: throw new IllegalStateException("Illegal tick value");
				}
				return result;
			}

			public void remove() {
				throw new UnsupportedOperationException();
			}
		}

		return new TickIterator();
	}
}
