package org.omnetpp.scave.editors.ui;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.omnetpp.common.engine.MatchExpression;
import org.omnetpp.common.engine.QuantityFormatter;
import org.omnetpp.common.engine.QuantityFormatter.Options;

/**
 *
 */
public class QuantityFormattingRule {
    public String name;
    public boolean enabled = true;
    public String expression;
    public MatchExpression matcher;
    public Options options;
    public String testInput;

    public QuantityFormattingRule(String name, boolean enabled, String expression, Options options, String testInput) {
        this.name = name;
        this.enabled = enabled;
        this.options = options;
        this.testInput = testInput;
        setExpression(expression);
    }

    public void setExpression(String expression) {
        this.expression = expression;
        try {
            this.matcher = new MatchExpression(expression, false, false, false);
        }
        catch (Exception e) {
            //TODO
            this.matcher = new MatchExpression("", false, false, false);;
        }
    }

    public QuantityFormattingRule getCopy() {
        return new QuantityFormattingRule(name, enabled, expression, new Options(options), testInput);
    }

    public String computeTestOutput() {
        if (testInput == null)
            return null;
        else {
            Pattern pattern = Pattern.compile("([-\\.\\d]+) *([^\\d]+)?");
            Matcher matcher = pattern.matcher(testInput.strip());
            if (matcher.find()) {
                String valueText = matcher.group(1);
                String unit = matcher.groupCount() > 1 ? matcher.group(2) : null;
                QuantityFormatter quantityFormatter = new QuantityFormatter(options);
                if (valueText.equals(""))
                    return "";
                else {
                    try {
                        double value = Double.parseDouble(valueText);
                        QuantityFormatter.Output output = quantityFormatter.formatQuantity(value, unit);
                        return output.getText();
                    }
                    catch (RuntimeException e) {
                        // void
                        return null;
                    }
                }
            }
            else
                return null;
        }
    }
}