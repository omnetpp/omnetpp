package com.simulcraft.test.gui.recorder;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.widgets.Widget;

import org.omnetpp.common.util.StringUtils;

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
        simplifySequence();
        makeVariablesUnique();

        // a "chain" is an expression like: "object.method1().method2().method3();"
        String text = "";
        int endOfCurrentChain = 0;
        for (int i = 0; i < list.size(); i++) {
            JavaExpr expr = list.get(i);
            if (endOfCurrentChain == i) {
                text = addSemicolonIfNeeded(text) + "\n";
                endOfCurrentChain = findEndOfChain(i);
                JavaExpr lastExprInChain = list.get(endOfCurrentChain-1);
                if (needsVariable(lastExprInChain))
                    text += lastExprInChain.getVariableType() + " " + lastExprInChain.getSuggestedVariableName() + " = ";
                if (expr.getCalledOn() != null)
                    text += expr.getCalledOn().getSuggestedVariableName();
            }

            if (expr.getCalledOn() != null)
                text += ".";
            text += expr.getJavaCode();
        }
        text = addSemicolonIfNeeded(text).trim() + "\n";
        return text;
    }
    
    protected boolean needsVariable(JavaExpr expr) {
        // needs variable if called from other place than immediately following expression
        JavaExpr prev = null;
        for (JavaExpr e : list) {
            if (e.getCalledOn() == expr && prev != expr) {
                Assert.isTrue(expr.getSuggestedVariableName() != null); // if needs variable, must have a variable name!
                return true;
            }
            prev = e;
        }
        return false;
    }
    
    protected int findEndOfChain(int startIndex) {
        for (int i = startIndex; i < list.size(); i++) {
            if (i+1 < list.size() && list.get(i+1).getCalledOn() != list.get(i))
                return i+1;
            if (needsVariable(list.get(i)))
                return i+1;
        }
        return list.size();
    }

    protected String addSemicolonIfNeeded(String text) {
        String lastLine = StringUtils.substringAfterLast(text, "\n");
        String lastLineWithoutComments = lastLine.replaceFirst("//[^\"]*$", "").trim(); // for this to work, comments should not contain quotes
        if (!StringUtils.isEmpty(lastLineWithoutComments) && !lastLineWithoutComments.endsWith(";"))
            return text + ";";
        else 
            return text;
    }


    protected void simplifySequence() {
        // replace "click, click, doubleClick" with "doubleClick"
        for (int i=0; i<list.size(); i++) {
            if (i<list.size()-2 && 
                    list.get(i).getJavaCode().equals("click()") &&
                    list.get(i+1).getJavaCode().equals("click()") &&
                    list.get(i+2).getJavaCode().equals("doubleClick()") &&
                    list.get(i).getCalledOn()==list.get(i+1).getCalledOn() &&
                    list.get(i+1).getCalledOn()==list.get(i+2).getCalledOn()) 
            {
                list.remove(i+1);
                list.remove(i);
            }
        }
    }
    
    protected void makeVariablesUnique() {
        Set<String> varnames = new HashSet<String>();
        for (JavaExpr expr : list) {
            if (needsVariable(expr)) {
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
    }

    @Override
    public String toString() {
        return "len=" + list.size() + " quality=" + getQuality() + ": " + generateCode().trim();
    }
}
