package org.omnetpp.common.eventlog;

import org.eclipse.jface.dialogs.IInputValidator;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.engine.Expression;
import org.omnetpp.common.engine.UnitConversion;
import org.omnetpp.common.engine.Value;
import org.omnetpp.common.ui.InputDialog;
import org.omnetpp.eventlog.engine.IEventLog;

public class GotoSimulationTimeDialog extends InputDialog {
    private BigDecimal baseSimulationTime;

    public GotoSimulationTimeDialog(final IEventLog eventLog, final BigDecimal baseSimulationTime) {
        super(null, "Go to Simulation Time", "Enter an absolute simulation time, or start with a '+' or '-' sign to indicate relative positioning. Time units and expressions are also accepted.", null, new IInputValidator() {
            public String isValid(String newText) {
                try {
                    BigDecimal simulationTime = getSimulationTime(baseSimulationTime, newText);
                    if (simulationTime.less(BigDecimal.getZero()))
                        return "The resulting simulation time is negative: " + simulationTime;
                    else if (simulationTime.greater(eventLog.getLastEvent().getSimulationTime()))
                        return "The resulting simulation time is beyond the end: " + simulationTime;
                    else
                        return null;
                }
                catch (Exception x) {
                    return "Expression does not evaluate to a simulation time";
                }
            }
        });
        this.baseSimulationTime = baseSimulationTime;
    }

    public BigDecimal getSimulationTime() {
        return getSimulationTime(baseSimulationTime, getValue());
    }

    public static BigDecimal getSimulationTime(BigDecimal baseSimulationTime, String originalValue) {
        boolean relative;
        String parsedValue;
        // check sign
        if (originalValue.startsWith("+") || originalValue.startsWith("-")) {
            relative = true;
            parsedValue = "0s" + originalValue;
        }
        else {
            relative = false;
            parsedValue = originalValue;
        }
        // parse
        Expression expression = new Expression();
        expression.parse(parsedValue);
        Value value = expression.evaluate();
        BigDecimal simulationTime = new BigDecimal(value.getDblunit() == null ? value.getDbl() : UnitConversion.convertUnit(value.getDbl(), value.getDblunit(), "s"));
        // relative vs. absolute
        if (relative)
            return baseSimulationTime.add(simulationTime);
        else
            return simulationTime;
    }
}
