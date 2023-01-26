/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.jface.preference.IPreferenceStore;
import org.omnetpp.common.engine.QuantityFormatter;
import org.omnetpp.common.engine.QuantityFormatter.Options;
import org.omnetpp.common.engine.StringVector;

public class QuantityFormatterUtils {

    private static final String UNIT_SEPARATOR = "unitSeparator";
    private static final String INCLUDE_UNIT = "includeUnit";
    private static final String MINUS_SIGN = "minusSign";
    private static final String PLUS_SIGN = "plusSign";
    private static final String EXPONENT_SIGN_MODE = "exponentSignMode";
    private static final String SIGN_MODE = "signMode";
    private static final String EXPONENT_MARK = "exponentMark";
    private static final String APPROXIMATION_MARK = "approximationMark";
    private static final String DECIMAL_SEPARATOR = "decimalSeparator";
    private static final String GROUP_SEPARATOR = "groupSeparator";
    private static final String ALLOWED_OUTPUT_UNITS = "allowedOutputUnits";
    private static final String OUTPUT_UNIT_MODE = "outputUnitMode";
    private static final String MAX_SIGNIFICANT_DIGITS = "maxSignificantDigits";
    private static final String MIN_SIGNIFICANT_DIGITS = "minSignificantDigits";
    private static final String SCIENTIFIC_NOTATION_EXPONENT_LIMIT = "scientificNotationExponentLimit";
    private static final String SCIENTIFIC_EXPONENT_MODE = "scientificExponentMode";
    private static final String NOTATION_MODE = "notationMode";
    private static final String NUM_ACCURATE_DIGITS = "numAccurateDigits";

    public static void saveToPreferenceStore(IPreferenceStore prefs, String prefix, Options options) {
        prefs.setValue(prefix + NUM_ACCURATE_DIGITS, options.getNumAccurateDigits());
        prefs.setValue(prefix + NOTATION_MODE, options.getNotationMode().swigValue());
        prefs.setValue(prefix + SCIENTIFIC_EXPONENT_MODE, options.getScientificExponentMode().swigValue());
        prefs.setValue(prefix + SCIENTIFIC_NOTATION_EXPONENT_LIMIT, options.getScientificNotationExponentLimit());
        prefs.setValue(prefix + MIN_SIGNIFICANT_DIGITS, options.getMinSignificantDigits());
        prefs.setValue(prefix + MAX_SIGNIFICANT_DIGITS, options.getMaxSignificantDigits());
        prefs.setValue(prefix + OUTPUT_UNIT_MODE, options.getOutputUnitMode().swigValue());
        prefs.setValue(prefix + ALLOWED_OUTPUT_UNITS, StringUtils.join(options.getAllowedOutputUnits().toArray(), ","));
        prefs.setValue(prefix + GROUP_SEPARATOR, options.getGroupSeparator());
        prefs.setValue(prefix + DECIMAL_SEPARATOR, options.getDecimalSeparator());
        prefs.setValue(prefix + APPROXIMATION_MARK, options.getApproximationMark());
        prefs.setValue(prefix + EXPONENT_MARK, options.getExponentMark());
        prefs.setValue(prefix + SIGN_MODE, options.getSignMode().swigValue());
        prefs.setValue(prefix + EXPONENT_SIGN_MODE, options.getExponentSignMode().swigValue());
        prefs.setValue(prefix + PLUS_SIGN, options.getPlusSign());
        prefs.setValue(prefix + MINUS_SIGN, options.getMinusSign());
        prefs.setValue(prefix + INCLUDE_UNIT, options.getIncludeUnit());
        prefs.setValue(prefix + UNIT_SEPARATOR, options.getUnitSeparator());
    }

    public static Options loadFromPreferenceStore(IPreferenceStore prefs, String prefix, Options options) {
        if (prefs.contains(prefix + NUM_ACCURATE_DIGITS))
            options.setNumAccurateDigits(prefs.getInt(prefix + NUM_ACCURATE_DIGITS));
        if (prefs.contains(prefix + NOTATION_MODE))
            options.setNotationMode(QuantityFormatter.NotationMode.swigToEnum(prefs.getInt(prefix + NOTATION_MODE)));
        if (prefs.contains(prefix + SCIENTIFIC_EXPONENT_MODE))
            options.setScientificExponentMode(QuantityFormatter.ScientificExponentMode.swigToEnum(prefs.getInt(prefix + SCIENTIFIC_EXPONENT_MODE)));
        if (prefs.contains(prefix + SCIENTIFIC_NOTATION_EXPONENT_LIMIT))
            options.setScientificNotationExponentLimit(prefs.getInt(prefix + SCIENTIFIC_NOTATION_EXPONENT_LIMIT));
        if (prefs.contains(prefix + MIN_SIGNIFICANT_DIGITS))
            options.setMinSignificantDigits(prefs.getInt(prefix + MIN_SIGNIFICANT_DIGITS));
        if (prefs.contains(prefix + MAX_SIGNIFICANT_DIGITS))
            options.setMaxSignificantDigits(prefs.getInt(prefix + MAX_SIGNIFICANT_DIGITS));
        if (prefs.contains(prefix + OUTPUT_UNIT_MODE))
            options.setOutputUnitMode(QuantityFormatter.OutputUnitMode.swigToEnum(prefs.getInt(prefix + OUTPUT_UNIT_MODE)));
        if (prefs.contains(prefix + ALLOWED_OUTPUT_UNITS))
            options.setAllowedOutputUnits(StringVector.fromArray((prefs.getString(prefix + ALLOWED_OUTPUT_UNITS)).split(",")));
        if (prefs.contains(prefix + GROUP_SEPARATOR))
            options.setGroupSeparator(prefs.getString(prefix + GROUP_SEPARATOR));
        if (prefs.contains(prefix + DECIMAL_SEPARATOR))
            options.setDecimalSeparator(prefs.getString(prefix + DECIMAL_SEPARATOR));
        if (prefs.contains(prefix + APPROXIMATION_MARK))
            options.setApproximationMark(prefs.getString(prefix + APPROXIMATION_MARK));
        if (prefs.contains(prefix + EXPONENT_MARK))
            options.setExponentMark(prefs.getString(prefix + EXPONENT_MARK));
        if (prefs.contains(prefix + SIGN_MODE))
            options.setSignMode(QuantityFormatter.SignMode.swigToEnum(prefs.getInt(prefix + SIGN_MODE)));
        if (prefs.contains(prefix + EXPONENT_SIGN_MODE))
            options.setExponentSignMode(QuantityFormatter.SignMode.swigToEnum(prefs.getInt(prefix + EXPONENT_SIGN_MODE)));
        if (prefs.contains(prefix + PLUS_SIGN))
            options.setPlusSign(prefs.getString(prefix + PLUS_SIGN));
        if (prefs.contains(prefix + MINUS_SIGN))
            options.setMinusSign(prefs.getString(prefix + MINUS_SIGN));
        if (prefs.contains(prefix + INCLUDE_UNIT))
            options.setIncludeUnit(prefs.getBoolean(prefix + INCLUDE_UNIT));
        if (prefs.contains(prefix + UNIT_SEPARATOR))
            options.setUnitSeparator(prefs.getString(prefix + UNIT_SEPARATOR));
        return options;
    }

    public static void testSerializationWith(String prefix, Options options) {
//      PreferenceStore tmp = new PreferenceStore();
//      QuantityFormatterRegistry.saveToPreferenceStore(tmp, prefix, options);
//      tmp.save(new StringStnull, "testing");
//      Options options2 = QuantityFormatterRegistry.deserializeFromMap(map);
//      Map<String, Object> map2 = QuantityFormatterRegistry.serializeToMap(options2);
//      Assert.isTrue(map2.equals(map), "QuantityFormatter.Options serialization/deserialization inconsistency");
  }

}
