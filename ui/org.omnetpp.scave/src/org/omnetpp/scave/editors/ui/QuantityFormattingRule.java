package org.omnetpp.scave.editors.ui;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.MatchExpression;
import org.omnetpp.common.engine.MatchExpression.Matchable;
import org.omnetpp.common.engine.QuantityFormatter;
import org.omnetpp.common.engine.QuantityFormatter.Options;
import org.omnetpp.common.engine.UnitConversion;
import org.omnetpp.common.util.StringUtils;

/**
 * Formatting rule for numbers / quantities in the Browse Data page
 * and potentially elsewhere.
 */
public class QuantityFormattingRule {
    private static final String DEFAULT_TESTINPUT = "1234567.8, 12.34567ms";

    private static int lastRuleNumber = 0;

    private String name = "";
    private boolean enabled = true;
    private String expression = "";
    private boolean isExpressionBogus = false;
    private MatchExpression matcher = null;
    private Options options = new Options();
    private String testInput = "";

    public QuantityFormattingRule() {
        this("Rule " + ++lastRuleNumber);
    }

    public QuantityFormattingRule(String name) {
        this(name, true, "", new Options(), DEFAULT_TESTINPUT);
    }

    public QuantityFormattingRule(String name, boolean enabled, String expression, Options options, String testInput) {
        setName(name);
        setEnabled(enabled);
        setExpression(expression);
        setOptions(options);
        setTestInput(testInput);
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        Assert.isNotNull(name);
        this.name = name;
        if (name.matches("Rule \\d+"))
            lastRuleNumber = Math.max(lastRuleNumber, Integer.parseInt(name.substring(5)));
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
            this.matcher = expression.isBlank() || expression.equals("*") ? null : new MatchExpression(expression, false, false, false);
            this.isExpressionBogus = false;
        }
        catch (Exception e) {
            this.matcher = null;
            this.isExpressionBogus = true;
        }
    }

    public boolean isCatchAll() {
        return matcher == null;
    }

    public boolean isExpressionBogus() {
        return isExpressionBogus;
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
        String[] items = getTestInput().split(",");
        QuantityFormatter quantityFormatter = new QuantityFormatter(getOptions());
        for (int i = 0; i < items.length; i++) {
            try {
                String item = items[i].strip();
                if (!item.isBlank()) {
                    String unit = UnitConversion.parseQuantityForUnit(item);
                    double value = unit.isEmpty() ? UnitConversion.parseQuantity(item) : UnitConversion.parseQuantity(item, unit);
                    QuantityFormatter.Output output = unit.isEmpty() ? quantityFormatter.formatQuantity(value, null) : quantityFormatter.formatQuantity(value, unit);
                    items[i] = output.getText();
                }
            }
            catch (Exception e) {
                items[i] = "<" + e.getMessage() + ">";
            }
        }
        return StringUtils.join((Object[])items, ", ");
    }
}