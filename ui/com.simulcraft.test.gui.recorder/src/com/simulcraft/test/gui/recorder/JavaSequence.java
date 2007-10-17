package com.simulcraft.test.gui.recorder;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

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

    /**
     * Returns the expression which produces the given UI object,
     * or returns null.
     */
    public JavaExpr lookup(Object uiObject) {
        for (JavaExpr expr : list)
            if (expr.getResultUIObject() == uiObject)
                return expr;
        return null;
    }
    
    /**
     * Release stale (disposed) UI objects, held by member JavaExpr objects.
     */
    public void cleanup() {
        for (JavaExpr expr : list)
            if (expr.getResultUIObject() instanceof Widget && ((Widget)expr.getResultUIObject()).isDisposed())
                expr.releaseUIObject();
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
        makeVariablesUnique();

        String text = "";
        for (JavaExpr expr : list) {
            //TODO if (needsVariable(expr))...
            if (expr.getSuggestedVariableName() != null)
                text += "SomeAccess " + expr.getSuggestedVariableName() + " = ";
            if (expr.getCalledOn() != null)
                text += expr.getCalledOn().getSuggestedVariableName() + ".";
            text += expr.getJavaCode();
            text += ";\n";
        }
        return text;
    }
    
    protected void makeVariablesUnique() {
        Set<String> varnames = new HashSet<String>();
        for (JavaExpr expr : list) {
            String varname = expr.getSuggestedVariableName();
            if (varname != null) {
                if (varnames.contains(varname)) {
                    int k = 2;
                    while (varnames.contains(varname+k))
                        k++;
                    varname = varname+k;
                    expr.setSuggestedVariableName(varname);
                }
                varnames.add(varname);
            }
        }
    }

    protected boolean needsVariable(JavaExpr expr) {
        // needs variable if called from other place than immediately following expression
        JavaExpr prev = null;
        for (JavaExpr e : list) {
            if (e.getCalledOn() == expr && prev != expr)
                return true;
            prev = e;
        }
        return false;
    }
    
    @Override
    public String toString() {
        return "len=" + list.size() + " quality=" + getQuality() + ": " + generateCode().trim();
    }
}
