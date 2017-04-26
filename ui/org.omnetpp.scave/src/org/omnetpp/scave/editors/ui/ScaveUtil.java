/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.Locale;

import org.omnetpp.common.engine.BigDecimal;

/**
 * TODO
 *
 * @author andras
 */
public class ScaveUtil {
    /**
     * Convert the given double to string, printing at most the given number
     * of significant digits.
     */
    public static String formatNumber(double d, int numSignificantDigits) {
        return (d == Math.floor(d)) ? String.format(Locale.US, "%,.0f", d) : String.format(Locale.US, "%,." + numSignificantDigits + "f", d);
        //return new DecimalFormat("###,###,###,###,###,###,###,##0." + repeat('0', numSignificantDigits)).format(d);
        //return Common.formatDouble(d, numSignificantDigits);
        //return String.format(Locale.US, "%." + numSignificantDigits + "g", d); // differs from C's printf because this one leaves trailing zeros in
    }

    /**
     * Convert the given double to string, printing at most the given number
     * of significant digits. See formatNumber(double d, int numSignificantDigits).
     */
    public static String formatNumber(BigDecimal d, int numSignificantDigits) {
        // Workaround: BigDecimal can only convert itself to string with full precision, so we
        // convert it via double. Double has ~15..17 digits of precision, so if numSignificantDigits
        // is near that, we use BigDecimal's full-precision toString() method.
        return numSignificantDigits < 15 ? formatNumber(d.doubleValue(), numSignificantDigits) : d.toString();
    }

}
