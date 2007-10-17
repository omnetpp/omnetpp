package com.simulcraft.test.gui.recorder;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.widgets.Widget;

public class JavaSequence {
    private List<JavaExpr> list = new ArrayList<JavaExpr>();

    public JavaSequence() {
    }

    public void merge(JavaSequence other) {
        Assert.isTrue(other != null);
        for (JavaExpr expr : other.list)
            add(expr);
    }
    
    public void add(JavaExpr expr) {
        list.add(expr);
    }

    public boolean isEmpty() {
        return list.isEmpty();
    }
    
    public void clear() {
        list.clear();
    }

    public JavaExpr getLast() {
        return list.get(list.size()-1); //XXX needed?
    }
    
    /**
     * Returns the expression which produces the given UI object,
     * or returns null.
     */
    public JavaExpr lookup(Object uiObject) {
        for (JavaExpr expr : list)
            if (expr.getUiObject() == uiObject)
                return expr;
        return null;
    }
    
    /**
     * Release stale (disposed) UI objects, held by member JavaExpr objects.
     */
    public void cleanup() {
        for (JavaExpr expr : list)
            if (expr.getUiObject() instanceof Widget && ((Widget)expr.getUiObject()).isDisposed())
                expr.setUiObject(null);
    }

    /**
     * Returns the quality of this Java code sequence (0.0=worst, 1.0=best)
     */
    public double getQuality() {
        double q = 1;
        for (JavaExpr expr : list)
            q *= expr.getQuality();
        return q;
    }
    
    /**
     * Generate Java code from the JavaExpr objects contained. 
     */
    public String generateCode() {
        // preliminary, simplified version
        String text = "";
        for (JavaExpr expr : list) {
            if (expr.getMethodOf() != null)
                text += ".";
            else if (text.length() > 0)
                text += ";\n";
            text += expr.getJavaCode();
        }
        text += ";\n";
        return text;
    }
    
    @Override
    public String toString() {
        return "len=" + list.size() + " quality=" + getQuality() + ": " + generateCode().trim();
    }
}
