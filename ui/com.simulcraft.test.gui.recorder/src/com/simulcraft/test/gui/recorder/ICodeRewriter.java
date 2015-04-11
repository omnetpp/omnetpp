/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.recorder;


/**
 * Rewriters may modify the code to make it more straightforward, etc.
 *
 * @author Andras
 */
public interface ICodeRewriter {
    /**
     * May modify the code to make it more straightforward, etc.
     */
    void rewrite(JavaSequence list);
}
