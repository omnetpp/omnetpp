/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

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
