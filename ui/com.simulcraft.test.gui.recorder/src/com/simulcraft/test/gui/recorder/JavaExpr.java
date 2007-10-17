package com.simulcraft.test.gui.recorder;

public class JavaExpr {
    private String javaCode;
    private double quality;
    private JavaExpr methodOf;
    private Object uiObject;
    private String suggestedVariableName;

    public JavaExpr(String javaCode, double quality) {
        this.javaCode = javaCode;
        this.quality = quality;
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

    public JavaExpr getMethodOf() {
        return methodOf;
    }

    public void setMethodOf(JavaExpr methodOf) {
        this.methodOf = methodOf;
    }

    public Object getUiObject() {
        return uiObject;
    }

    public void setUiObject(Object uiObject) {
        this.uiObject = uiObject;
    }

    public String getSuggestedVariableName() {
        return suggestedVariableName;
    }

    public void setSuggestedVariableName(String suggestedVariableName) {
        this.suggestedVariableName = suggestedVariableName;
    }
}
