package org.omnetpp.common.eventlog;

import org.eclipse.jface.dialogs.IInputValidator;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.engine.Expression;
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

    public static BigDecimal getSimulationTime(BigDecimal baseSimulationTime, String expr) {
        boolean relative = false;

        // check sign
        if (expr.startsWith("+")) {
            relative = true;
            expr = expr.substring(1);
        } 
        else if (expr.startsWith("-")) {
            relative = true;
        }
        
        // parse
        Expression expression = new Expression();
        expression.parse(expr);
        double result = expr.matches(".*[a-zA-Z].*") ? expression.doubleValue("s") : expression.doubleValue();  
        BigDecimal simulationTime = new BigDecimal(result);
        // relative vs. absolute
        if (relative)
            return baseSimulationTime.add(simulationTime);
        else
            return simulationTime;
    }
}
