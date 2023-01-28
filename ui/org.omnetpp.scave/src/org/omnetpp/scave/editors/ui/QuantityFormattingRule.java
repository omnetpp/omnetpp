package org.omnetpp.scave.editors.ui;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.MatchExpression;
import org.omnetpp.common.engine.MatchExpression.Matchable;
import org.omnetpp.common.engine.QuantityFormatter;
import org.omnetpp.common.engine.QuantityFormatter.Options;

/**
 *
 */
public class QuantityFormattingRule {
    private String name = "";
    private boolean enabled = true;
    private String expression = "";
    private MatchExpression matcher = null;
    private Options options = new Options();
    private String testInput = "";

    public QuantityFormattingRule(String name) {
        this(name, true, "", new Options(), "");
    }

    public QuantityFormattingRule(String name, boolean enabled, String expression, Options options, String testInput) {
        setName(name);
        setEnabled(enabled);
        setOptions(options);
        setTestInput(testInput);
        setExpression(expression);
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        Assert.isNotNull(name);
        this.name = name;
    }

    public boolean isEnabled() {
        return enabled;
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    public String getExpression() {
        return expression;
    }

    public void setExpression(String expression) {
        Assert.isNotNull(expression);
        this.expression = expression;

        try {
            this.matcher = expression.isBlank() ? null : new MatchExpression(expression, false, false, false);
        }
        catch (Exception e) {
            this.matcher = null;
        }
    }

    public boolean matches(Matchable object) {
        return isEnabled() && (matcher == null || matcher.matches(object));
    }

    public Options getOptions() {
        return options;
    }

    public void setOptions(Options options) {
        Assert.isNotNull(options);
        this.options = options;
    }

    public String getTestInput() {
        return testInput;
    }

    public void setTestInput(String testInput) {
        Assert.isNotNull(testInput);
        this.testInput = testInput;
    }

    public QuantityFormattingRule getCopy() {
        return new QuantityFormattingRule(getName(), isEnabled(), getExpression(), new Options(getOptions()), getTestInput());
    }

    public String computeTestOutput() {
        Pattern pattern = Pattern.compile("([-\\.\\d]+) *([^\\d]+)?");
        Matcher matcher = pattern.matcher(getTestInput().strip());
        if (matcher.find()) {  //TODO "while" instead of "if" ?
            String valueText = matcher.group(1);
            String unit = matcher.groupCount() > 1 ? matcher.group(2) : null;
            QuantityFormatter quantityFormatter = new QuantityFormatter(getOptions());
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