package com.simulcraft.test.gui.recorder;


/**
 * @author Andras
 */
public interface IObjectRecognizer {
    /**
     * Last one is the expression's value
     */
    JavaSequence identifyObject(Object uiObject);
}
