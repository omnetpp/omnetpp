package com.simulcraft.test.gui.recorder;

import java.util.List;

/**
 * @author Andras
 */
public interface IObjectRecognizer {
    /**
     * Last one is the expression's value
     */
    List<JavaExpr> identifyObject(Object uiObject);
}
