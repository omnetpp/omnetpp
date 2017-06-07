package org.omnetpp.scave.script;

import java.util.Arrays;

import org.omnetpp.scave.computed.VectorOperation;

/**
 * Represents the "APPLY" command in analysis scripts
 *
 * @author andras
 */
//TODO resultType?
public class ApplyCommand  extends ScriptCommand {
    private VectorOperation operation;
    private Object[] parameters; // of the operation
    private String filterExpression; // WHERE clause
    
    public VectorOperation getOperation() {
        return operation;
    }
    
    public void setOperation(VectorOperation operation) {
        this.operation = operation;
    }
    
    public Object[] getParameters() {
        return parameters;
    }
    
    public void setParameters(Object[] parameters) {
        this.parameters = parameters;
    }
    
    public String getFilterExpression() {
        return filterExpression;
    }
    
    public void setFilterExpression(String filterExpression) {
        this.filterExpression = filterExpression;
    }

    @Override
    public String toString() {
        return "ApplyCommand [operation=" + operation.getName() + ", parameters=" + Arrays.toString(parameters) + ", filterExpression=" + filterExpression + "]";
    }
}
