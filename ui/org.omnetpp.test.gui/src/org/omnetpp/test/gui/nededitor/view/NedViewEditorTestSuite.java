/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.nededitor.view;


import junit.framework.Test;
import junit.framework.TestSuite;

public class NedViewEditorTestSuite
    extends TestSuite
{
    public NedViewEditorTestSuite() {
        addTestSuite(NedOutlineViewTest.class);
    }

    public static Test suite() {
        return new NedViewEditorTestSuite();
    }
}
