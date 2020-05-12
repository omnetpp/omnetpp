/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.pychart;

/**
 * Describes an action (which can be performed by the user), reported by
 * matplotlib to our backend. These are supposed to be put on a toolbar, as
 * [radio]buttons with icons.
 */
public class ActionDescription {

    public ActionDescription(String text, String tooltipText, String imageFile, String methodName) {
        this.text = text;
        this.tooltipText = tooltipText;
        this.imageFile = imageFile;
        this.methodName = methodName;
    }

    /** the text of the button (often not visible to users) */
    public String text;

    /** the tooltip shown on hover (where possible) */
    public String tooltipText;

    /** name of the image for the button (without the extension) */
    public String imageFile;

    /** name of the method in NavigationToolbar2 to call */
    public String methodName;
}
