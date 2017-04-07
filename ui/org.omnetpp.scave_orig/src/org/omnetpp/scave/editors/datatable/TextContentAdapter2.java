/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;

/**
 * Implementation of <code>IControlContentAdapter2</code> for text widgets.
 */
public class TextContentAdapter2 extends TextContentAdapter implements IControlContentAdapter2 {

    public void replaceControlContents(Control control, int start, int end, String text, int cursorPosition) {
        Text textControl = (Text)control;
        textControl.setSelection(start, end);
        textControl.insert(text);
        textControl.setSelection(start + cursorPosition, start + cursorPosition);
    }
}
