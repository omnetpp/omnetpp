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
import org.omnetpp.common.engine.UnitConversion;

public class QuantityFormatterUtils {

    private static final String UNIT_SEPARATOR = "unitSeparator";
    private static final String USE_UNIT_LONG_NAME = "useUnitLongName";
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
    private static final String ALLOW_ORIGINAL_UNIT = "allowOriginalUnit";
    private static final String CONVERT_ZERO_TO_BASE_UNIT = "convertZeroToBaseUnit";
    private static final String ALLOW_NONMETRIC_TIME_UNITS = "allowNonmetricTimeUnits";
    private static final String LOGARITHMIC_UNITS_POLICY = "logarithmicUnitsPolicy";
    private static final String BIT_BASED_UNITS_POLICY = "bitBasedUnitsPolicy";
    private static final String BINARY_PREFIX_POLICY = "binaryPrefixPolicy";
    private static final String KILOBYTE_THRESHOLD = "kilobyteThreshold";
    private static final String PREFER_WHOLE_NUMBERS = "preferSmallWholeNumbersForBitByte";

    public static void setPreferenceStoreDefaults(IPreferenceStore prefs, String prefix) {
        // this is necessary due to how the IPreferenceStore API works
        Options options = QuantityFormattingRule.makeDefaultOptions();
        prefs.setDefault(prefix + NUM_ACCURATE_DIGITS, options.getNumAccurateDigits());
        prefs.setDefault(prefix + NOTATION_MODE, options.getNotationMode().swigValue());
        prefs.setDefault(prefix + SCIENTIFIC_EXPONENT_MODE, options.getScientificExponentMode().swigValue());
        prefs.setDefault(prefix + SCIENTIFIC_NOTATION_EXPONENT_LIMIT, options.getScientificNotationExponentLimit());
        prefs.setDefault(prefix + MIN_SIGNIFICANT_DIGITS, options.getMinSignificantDigits());
        prefs.setDefault(prefix + MAX_SIGNIFICANT_DIGITS, options.getMaxSignificantDigits());
        prefs.setDefault(prefix + OUTPUT_UNIT_MODE, options.getOutputUnitMode().swigValue());
        prefs.setDefault(prefix + ALLOWED_OUTPUT_UNITS, StringUtils.join(options.getAllowedOutputUnits().toArray(), ","));
        prefs.setDefault(prefix + GROUP_SEPARATOR, options.getGroupSeparator());
        prefs.setDefault(prefix + DECIMAL_SEPARATOR, options.getDecimalSeparator());
        prefs.setDefault(prefix + APPROXIMATION_MARK, options.getApproximationMark());
        prefs.setDefault(prefix + EXPONENT_MARK, options.getExponentMark());
        prefs.setDefault(prefix + SIGN_MODE, options.getSignMode().swigValue());
        prefs.setDefault(prefix + EXPONENT_SIGN_MODE, options.getExponentSignMode().swigValue());
        prefs.setDefault(prefix + PLUS_SIGN, options.getPlusSign());
        prefs.setDefault(prefix + MINUS_SIGN, options.getMinusSign());
        prefs.setDefault(prefix + INCLUDE_UNIT, options.getIncludeUnit());
        prefs.setDefault(prefix + USE_UNIT_LONG_NAME, options.getUseUnitLongName());
        prefs.setDefault(prefix + UNIT_SEPARATOR, options.getUnitSeparator());

        UnitConversion.Options unitOptions = options.getUnitConversionOptions();
        prefs.setDefault(prefix + ALLOW_ORIGINAL_UNIT, unitOptions.getAllowOriginalUnit());
        prefs.setDefault(prefix + BIT_BASED_UNITS_POLICY, unitOptions.getBitBasedUnitsPolicy().swigValue());
        prefs.setDefault(prefix + BINARY_PREFIX_POLICY, unitOptions.getBinaryPrefixPolicy().swigValue());
        prefs.setDefault(prefix + PREFER_WHOLE_NUMBERS, unitOptions.getPreferSmallWholeNumbersForBitByte());
        prefs.setDefault(prefix + CONVERT_ZERO_TO_BASE_UNIT, unitOptions.getConvertZeroToBaseUnit());
        prefs.setDefault(prefix + ALLOW_NONMETRIC_TIME_UNITS, unitOptions.getAllowNonmetricTimeUnits());
        prefs.setDefault(prefix + LOGARITHMIC_UNITS_POLICY, unitOptions.getLogarithmicUnitsPolicy().swigValue());
        prefs.setDefault(prefix + KILOBYTE_THRESHOLD, unitOptions.getKilobyteThreshold());
    }


    public static void saveToPreferenceStore(IPreferenceStore prefs, String prefix, Options options) {
        setPreferenceStoreDefaults(prefs, prefix);
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
        prefs.setValue(prefix + USE_UNIT_LONG_NAME, options.getUseUnitLongName());
        prefs.setValue(prefix + UNIT_SEPARATOR, options.getUnitSeparator());

        UnitConversion.Options unitOptions = options.getUnitConversionOptions();
        prefs.setValue(prefix + ALLOW_ORIGINAL_UNIT, unitOptions.getAllowOriginalUnit());
        prefs.setValue(prefix + BIT_BASED_UNITS_POLICY, unitOptions.getBitBasedUnitsPolicy().swigValue());
        prefs.setValue(prefix + BINARY_PREFIX_POLICY, unitOptions.getBinaryPrefixPolicy().swigValue());
        prefs.setValue(prefix + PREFER_WHOLE_NUMBERS, unitOptions.getPreferSmallWholeNumbersForBitByte());
        prefs.setValue(prefix + CONVERT_ZERO_TO_BASE_UNIT, unitOptions.getConvertZeroToBaseUnit());
        prefs.setValue(prefix + ALLOW_NONMETRIC_TIME_UNITS, unitOptions.getAllowNonmetricTimeUnits());
        prefs.setValue(prefix + LOGARITHMIC_UNITS_POLICY, unitOptions.getLogarithmicUnitsPolicy().swigValue());
        prefs.setValue(prefix + KILOBYTE_THRESHOLD, unitOptions.getKilobyteThreshold());
    }

    public static Options loadFromPreferenceStore(IPreferenceStore prefs, String prefix, Options options) {
        setPreferenceStoreDefaults(prefs, prefix);
        options.setNumAccurateDigits(prefs.getInt(prefix + NUM_ACCURATE_DIGITS));
        options.setNotationMode(QuantityFormatter.NotationMode.swigToEnum(prefs.getInt(prefix + NOTATION_MODE)));
        options.setScientificExponentMode(QuantityFormatter.ScientificExponentMode.swigToEnum(prefs.getInt(prefix + SCIENTIFIC_EXPONENT_MODE)));
        options.setScientificNotationExponentLimit(prefs.getInt(prefix + SCIENTIFIC_NOTATION_EXPONENT_LIMIT));
        options.setMinSignificantDigits(prefs.getInt(prefix + MIN_SIGNIFICANT_DIGITS));
        options.setMaxSignificantDigits(prefs.getInt(prefix + MAX_SIGNIFICANT_DIGITS));
        options.setOutputUnitMode(QuantityFormatter.OutputUnitMode.swigToEnum(prefs.getInt(prefix + OUTPUT_UNIT_MODE)));
        options.setAllowedOutputUnits(StringVector.fromArray((prefs.getString(prefix + ALLOWED_OUTPUT_UNITS)).split(",")));
        options.setGroupSeparator(prefs.getString(prefix + GROUP_SEPARATOR));
        options.setDecimalSeparator(prefs.getString(prefix + DECIMAL_SEPARATOR));
        options.setApproximationMark(prefs.getString(prefix + APPROXIMATION_MARK));
        options.setExponentMark(prefs.getString(prefix + EXPONENT_MARK));
        options.setSignMode(QuantityFormatter.SignMode.swigToEnum(prefs.getInt(prefix + SIGN_MODE)));
        options.setExponentSignMode(QuantityFormatter.SignMode.swigToEnum(prefs.getInt(prefix + EXPONENT_SIGN_MODE)));
        options.setPlusSign(prefs.getString(prefix + PLUS_SIGN));
        options.setMinusSign(prefs.getString(prefix + MINUS_SIGN));
        options.setIncludeUnit(prefs.getBoolean(prefix + INCLUDE_UNIT));
        options.setUseUnitLongName(prefs.getBoolean(prefix + USE_UNIT_LONG_NAME));
        options.setUnitSeparator(prefs.getString(prefix + UNIT_SEPARATOR));

        UnitConversion.Options unitOptions = options.getUnitConversionOptions();
        unitOptions.setAllowOriginalUnit(prefs.getBoolean(prefix + ALLOW_ORIGINAL_UNIT));
        unitOptions.setBitBasedUnitsPolicy(UnitConversion.Preference.swigToEnum(prefs.getInt(prefix + BIT_BASED_UNITS_POLICY)));
        unitOptions.setBinaryPrefixPolicy(UnitConversion.Preference.swigToEnum(prefs.getInt(prefix + BINARY_PREFIX_POLICY)));
        unitOptions.setPreferSmallWholeNumbersForBitByte(prefs.getBoolean(prefix + PREFER_WHOLE_NUMBERS));
        unitOptions.setConvertZeroToBaseUnit(prefs.getBoolean(prefix + CONVERT_ZERO_TO_BASE_UNIT));
        unitOptions.setAllowNonmetricTimeUnits(prefs.getBoolean(prefix + ALLOW_NONMETRIC_TIME_UNITS));
        unitOptions.setLogarithmicUnitsPolicy(UnitConversion.Preference.swigToEnum(prefs.getInt(prefix + LOGARITHMIC_UNITS_POLICY)));
        unitOptions.setKilobyteThreshold(prefs.getDouble(prefix + KILOBYTE_THRESHOLD));

        return options;
    }

}
