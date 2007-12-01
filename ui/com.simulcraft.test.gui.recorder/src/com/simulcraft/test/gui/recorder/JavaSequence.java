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
    private int modificationCount = 0;

    public JavaSequence() {
    }

    public void merge(JavaSequence other) {
        Assert.isTrue(other != null);
        for (JavaExpr expr : other.list)
            add(expr);
    }
    
    public void add(JavaExpr expr) {
        list.add(expr);
        modificationCount++;
    }

    public boolean isEmpty() {
        return list.isEmpty();
    }
    
    public void clear() {
        list.clear();
    }

    public int length() {
        return list.size();
    }
    
    /**
     * Returns the given element. Returns null if out of bounds.
     */
    public JavaExpr get(int index) {
        return index<0 || index>=list.size() ? null : list.get(index);
    }

    /**
     * Convenience method for code rewriters.
     */
    public boolean endEquals(int index, String string) {
        JavaExpr expr = getEnd(index);
        return expr != null && expr.getJavaCode().equals(string);
    }

    /**
     * Convenience method for code rewriters.
     */
    public boolean endMatches(int index, String regex) {
        JavaExpr expr = getEnd(index);
        return expr != null && expr.getJavaCode().matches(regex);
    }

    /**
     * Returns an element, index being relative to the end of the list;
     * index must be negative. Examples: index=-1: last, index=-2: last but one, etc.
     * Returns null if out of bounds.
     */
    public JavaExpr getEnd(int index) {
        Assert.isTrue(index<0);
        return get(list.size() + index);
    }

    public void replaceEnd(int length, JavaSequence seq) {
        Assert.isTrue(length<=list.size());
        replaceEnd(-length, length, seq);
    }

    public void replaceEnd(int index, int length, JavaSequence seq) {
        Assert.isTrue(index < 0 && index >= -list.size());
        Assert.isTrue(index-length < 0 && index-length >= -list.size());
        int pos = index + list.size(); // normal index
        for (int i=0; i<length; i++)
            list.remove(pos);
        if (seq != null)
            list.addAll(pos, seq.list);
        modificationCount++;
    }
    
    /**
     * Returns a counter that gets incremented at every modification.
     * Can be used to detect if the sequence was modified since a
     * previous invocation.
     */
    public int getModificationCount() {
        return modificationCount;
    }
    
    /**
     * Returns the expression which produces the given UI object,
     * or returns null.
     */
    public JavaExpr lookupUIObject(Object uiObject) {
        for (JavaExpr expr : list)
            if (expr.getResultUIObject() == uiObject)
                return expr;
        return null;
    }
    
    /**
     * Release stale (disposed) UI objects, held by member JavaExpr objects.
     */
    public void forgetDisposedUIObjects() {
        for (JavaExpr expr : list)
            if (expr.getResultUIObject() instanceof Widget && ((Widget)expr.getResultUIObject()).isDisposed())
                expr.forgetUIObject();
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
        return generateCode(0, length());   
    }
    
    /**
     * Generate Java code from the JavaExpr objects contained. 
     */
    public String generateCode(int startIndex, int endIndex) {
        makeVariablesUnique();

        // a "chain" is an expression like: "object.method1().method2().method3();"
        String text = "";
        int endOfCurrentChain = 0;
        for (int i = startIndex; i < endIndex; i++) {
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
