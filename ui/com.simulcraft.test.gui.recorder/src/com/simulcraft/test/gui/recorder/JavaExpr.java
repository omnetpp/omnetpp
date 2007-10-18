package com.simulcraft.test.gui.recorder;

import org.omnetpp.common.util.StringUtils;

public class JavaExpr {
    private String javaCode;
    private double quality;
    private JavaExpr calledOn;
    private Object resultUIObject;
    private String suggestedVariableName;
    private String variableType;

    public JavaExpr(String javaCode, double quality, Object resultUIObject, String variableType) {
        this(javaCode, quality, resultUIObject, variableType, resultUIObject == null ? null : 
            StringUtils.toInstanceName(resultUIObject.getClass().getSimpleName()));
    }

    public JavaExpr(String javaCode, double quality, Object resultUIObject, String variableType, String suggestedVariableName) {
        this.javaCode = javaCode;
        this.quality = quality;
        this.resultUIObject = resultUIObject;
        this.variableType = variableType;
        this.suggestedVariableName = suggestedVariableName;
    }

    public String getJavaCode() {
        return javaCode;
    }

    /**
     * Quality of this guess, 0.0=worst, 1.0=best.
     */
    public double getQuality() {
        return quality;
    }

    public JavaExpr getCalledOn() {
        return calledOn;
    }

    public void setCalledOn(JavaExpr calledOn) {
        this.calledOn = calledOn;
    }

    public Object getResultUIObject() {
        return resultUIObject;
    }
    
    public void releaseUIObject() {
        resultUIObject = null;
    }

    public String getSuggestedVariableName() {
        return suggestedVariableName;
    }

    public void setSuggestedVariableName(String suggestedVariableName) {
        this.suggestedVariableName = suggestedVariableName;
    }

    public String getVariableType() {
        return variableType;
    }

    public void setVariableType(String variableType) {
        this.variableType = variableType;
    }

    @Override
    public String toString() {
        return "quality=" + getQuality() + ": " + getJavaCode().trim();
    }

}
