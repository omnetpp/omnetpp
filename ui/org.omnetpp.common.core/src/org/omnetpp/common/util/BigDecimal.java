package org.omnetpp.common.util;

import java.math.BigInteger;
import java.math.MathContext;
import java.math.RoundingMode;

/**
 * This class overrides the equals method of java.math.BigDecimal to return true
 * for all different representations of the same number.
 */
public class BigDecimal extends java.math.BigDecimal {
    private static final long serialVersionUID = 1L;

    public static final BigDecimal ZERO = new BigDecimal(0);
    public static final BigDecimal MINUS_ONE = new BigDecimal(-1);

    public BigDecimal(java.math.BigDecimal o) {
        super(o.unscaledValue(), o.scale());
    }

    public BigDecimal(BigInteger arg0, int arg1, MathContext arg2) {
        super(arg0, arg1, arg2);
    }

    public BigDecimal(BigInteger unscaledVal, int scale) {
        super(unscaledVal, scale);
    }

    public BigDecimal(BigInteger val, MathContext mc) {
        super(val, mc);
    }

    public BigDecimal(BigInteger val) {
        super(val);
    }

    public BigDecimal(char[] arg0, int arg1, int arg2, MathContext arg3) {
        super(arg0, arg1, arg2, arg3);
    }

    public BigDecimal(char[] in, int offset, int len) {
        super(in, offset, len);
    }

    public BigDecimal(char[] in, MathContext mc) {
        super(in, mc);
    }

    public BigDecimal(char[] in) {
        super(in);
    }

    public BigDecimal(double arg0, MathContext arg1) {
        super(arg0, arg1);
    }

    public BigDecimal(double val) {
        super(val);
    }

    public BigDecimal(int arg0, MathContext arg1) {
        super(arg0, arg1);
    }

    public BigDecimal(int val) {
        super(val);
    }

    public BigDecimal(long arg0, MathContext arg1) {
        super(arg0, arg1);
    }

    public BigDecimal(long val) {
        super(val);
    }

    public BigDecimal(String val, MathContext mc) {
        super(val, mc);
    }

    public BigDecimal(String val) {
        super(val);
    }

    public boolean less(BigDecimal o) {
        return compareTo(o) < 0;
    }

    public boolean lessOrEqual(BigDecimal o) {
        return compareTo(o) <= 0;
    }

    public boolean greater(BigDecimal o) {
        return compareTo(o) > 0;
    }

    public boolean greaterOrEqual(BigDecimal o) {
        return compareTo(o) >= 0;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (getClass() != obj.getClass())
            return false;
        // see class comment
        return compareTo((BigDecimal)obj) == 0;
    }

    @Override
    public BigDecimal abs() {
        return new BigDecimal(super.abs());
    }

    @Override
    public BigDecimal abs(MathContext mc) {
        return new BigDecimal(super.abs(mc));
    }

    @Override
    public BigDecimal add(java.math.BigDecimal arg0, MathContext arg1) {
        return new BigDecimal(super.add(arg0, arg1));
    }

    @Override
    public BigDecimal add(java.math.BigDecimal augend) {
        return new BigDecimal(super.add(augend));
    }

    @Override
    public BigDecimal divide(java.math.BigDecimal divisor, int scale, int roundingMode) {
        return new BigDecimal(super.divide(divisor, scale, roundingMode));
    }

    @Override
    public BigDecimal divide(java.math.BigDecimal divisor, int scale, RoundingMode roundingMode) {
        return new BigDecimal(super.divide(divisor, scale, roundingMode));
    }

    @Override
    public BigDecimal divide(java.math.BigDecimal divisor, int roundingMode) {
        return new BigDecimal(super.divide(divisor, roundingMode));
    }

    @Override
    public BigDecimal divide(java.math.BigDecimal divisor, MathContext mc) {
        return new BigDecimal(super.divide(divisor, mc));
    }

    @Override
    public BigDecimal divide(java.math.BigDecimal divisor, RoundingMode roundingMode) {
        return new BigDecimal(super.divide(divisor, roundingMode));
    }

    @Override
    public BigDecimal divide(java.math.BigDecimal arg0) {
        return new BigDecimal(super.divide(arg0));
    }

    @Override
    public java.math.BigDecimal[] divideAndRemainder(java.math.BigDecimal divisor, MathContext mc) {
        java.math.BigDecimal[] ds = super.divideAndRemainder(divisor, mc);
        BigDecimal[] rs = new BigDecimal[ds.length];
        for (int i = 0; i < ds.length; i++) {
            java.math.BigDecimal d = ds[i];
            rs[i] = new BigDecimal(d);
        }
        return rs;
    }

    @Override
    public java.math.BigDecimal[] divideAndRemainder(java.math.BigDecimal divisor) {
        java.math.BigDecimal[] ds = super.divideAndRemainder(divisor);
        BigDecimal[] rs = new BigDecimal[ds.length];
        for (int i = 0; i < ds.length; i++) {
            java.math.BigDecimal d = ds[i];
            rs[i] = new BigDecimal(d);
        }
        return rs;
    }

    @Override
    public BigDecimal divideToIntegralValue(java.math.BigDecimal arg0, MathContext arg1) {
        return new BigDecimal(super.divideToIntegralValue(arg0, arg1));
    }

    @Override
    public BigDecimal divideToIntegralValue(java.math.BigDecimal divisor) {
        return new BigDecimal(super.divideToIntegralValue(divisor));
    }

    @Override
    public BigDecimal max(java.math.BigDecimal val) {
        return new BigDecimal(super.max(val));
    }

    @Override
    public BigDecimal min(java.math.BigDecimal val) {
        return new BigDecimal(super.min(val));
    }

    @Override
    public BigDecimal movePointLeft(int n) {
        return new BigDecimal(super.movePointLeft(n));
    }

    @Override
    public BigDecimal movePointRight(int n) {
        return new BigDecimal(super.movePointRight(n));
    }

    @Override
    public BigDecimal multiply(java.math.BigDecimal multiplicand,
            MathContext mc) {
        return new BigDecimal(super.multiply(multiplicand, mc));
    }

    @Override
    public BigDecimal multiply(java.math.BigDecimal multiplicand) {
        return new BigDecimal(super.multiply(multiplicand));
    }

    @Override
    public BigDecimal negate() {
        return new BigDecimal(super.negate());
    }

    @Override
    public BigDecimal negate(MathContext mc) {
        return new BigDecimal(super.negate(mc));
    }

    @Override
    public BigDecimal plus() {
        return new BigDecimal(super.plus());
    }

    @Override
    public BigDecimal plus(MathContext mc) {
        return new BigDecimal(super.plus(mc));
    }

    @Override
    public BigDecimal pow(int arg0, MathContext arg1) {
        return new BigDecimal(super.pow(arg0, arg1));
    }

    @Override
    public BigDecimal pow(int n) {
        return new BigDecimal(super.pow(n));
    }

    @Override
    public BigDecimal remainder(java.math.BigDecimal divisor, MathContext mc) {
        return new BigDecimal(super.remainder(divisor, mc));
    }

    @Override
    public BigDecimal remainder(java.math.BigDecimal divisor) {
        return new BigDecimal(super.remainder(divisor));
    }

    @Override
    public BigDecimal round(MathContext mc) {
        return new BigDecimal(super.round(mc));
    }

    @Override
    public BigDecimal scaleByPowerOfTen(int n) {
        return new BigDecimal(super.scaleByPowerOfTen(n));
    }

    @Override
    public BigDecimal setScale(int arg0, int arg1) {
        return new BigDecimal(super.setScale(arg0, arg1));
    }

    @Override
    public BigDecimal setScale(int newScale, RoundingMode roundingMode) {
        return new BigDecimal(super.setScale(newScale, roundingMode));
    }

    @Override
    public BigDecimal setScale(int newScale) {
        return new BigDecimal(super.setScale(newScale));
    }

    @Override
    public BigDecimal sqrt(MathContext arg0) {
        return new BigDecimal(super.sqrt(arg0));
    }

    @Override
    public BigDecimal stripTrailingZeros() {
        return new BigDecimal(super.stripTrailingZeros());
    }

    @Override
    public BigDecimal subtract(java.math.BigDecimal subtrahend, MathContext mc) {
        return new BigDecimal(super.subtract(subtrahend, mc));
    }

    @Override
    public BigDecimal subtract(java.math.BigDecimal subtrahend) {
        return new BigDecimal(super.subtract(subtrahend));
    }

    @Override
    public BigDecimal ulp() {
        return new BigDecimal(super.ulp());
    }

    public static BigDecimal valueOf(long val) {
        return new BigDecimal(java.math.BigDecimal.valueOf(val));
    }

    public static BigDecimal valueOf(double val) {
        return new BigDecimal(java.math.BigDecimal.valueOf(val));
    }
}
