package org.omnetpp.common.eventlog;

import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.engine.Expression;
import org.omnetpp.eventlog.engine.IEventLog;

public class GotoSimulationTimeDialog extends InputDialog {
    private BigDecimal baseSimulationTime;

    public GotoSimulationTimeDialog(final IEventLog eventLog, final BigDecimal baseSimulationTime) {
        super(null, "Goto simulation time", "Please enter an absolute simulation time or start with a '+' or '-' sign to indicate relative positioning.", null, new IInputValidator() {
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
            parsedValue = "0" + originalValue;
        }
        else {
            relative = false;
            parsedValue = originalValue;
        }
        // parse
        BigDecimal simulationTime = null;
        try {
            simulationTime = BigDecimal.parse(parsedValue);
        }
        catch (Exception e) {
            Expression expression = new Expression();
            expression.parse(parsedValue);
            simulationTime = new BigDecimal(expression.evaluate().getDbl());
        }
        // relative vs. absolute
        if (relative)
            return baseSimulationTime.add(simulationTime);
        else
            return simulationTime;
    }
}
