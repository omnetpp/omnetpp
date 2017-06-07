package org.omnetpp.scave.script;

import org.omnetpp.scave.model.ResultType;

/**
 * Represents the "ADD" command in analysis scripts
 *
 * @author andras
 */
public class AddCommand extends ScriptCommand {
    private ResultType resultType;
    private String filterExpression; // WHERE clause

    public void setResultType(ResultType resultType) {
        this.resultType = resultType;
    }

    public ResultType getResultType() {
        return resultType;
    }

    public void setFilterExpression(String filterExpression) {
        this.filterExpression = filterExpression;
    }


    public String getFilterExpression() {
        return filterExpression;
    }
    
    @Override
    public String toString() {
        return "AddCommand [resultType=" + resultType + ", filterExpression=" + filterExpression + "]";
    }

}
