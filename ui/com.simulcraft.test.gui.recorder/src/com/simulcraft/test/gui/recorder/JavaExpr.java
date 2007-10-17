package com.simulcraft.test.gui.recorder;

import org.omnetpp.common.util.StringUtils;

public class JavaExpr {
    private String javaCode;
    private double quality;
    private JavaExpr calledOn;
    private Object resultUIObject;
    private String suggestedVariableName;

    public JavaExpr(String javaCode, double quality, Object resultUIObject) {
        this(javaCode, quality, resultUIObject, resultUIObject == null ? null : 
            StringUtils.toInstanceName(resultUIObject.getClass().getSimpleName()));
    }

    public JavaExpr(String javaCode, double quality, Object resultUIObject, String suggestedVariableName) {
        this.javaCode = javaCode;
        this.quality = quality;
        this.resultUIObject = resultUIObject;
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

    @Override
    public String toString() {
        return "quality=" + getQuality() + ": " + getJavaCode().trim();
    }


}
