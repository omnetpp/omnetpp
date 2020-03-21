/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.virtualtable;

import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.graphics.Image;

/**
 * This class serves debugging purposes.
 */
public class LongVirtualTableRowRenderer extends AbstractRowRenderer<Long> {
    @Override
    public void setInput(Object input) {
        // void
    }

    @Override
    public String getTooltipText(Long element) {
        return null;
    }

    @Override
    public int getIndentation(Long element, int index) {
        return 5;
    }

    @Override
    public Image getImage(Long element, int index) {
        return null;
    }

    @Override
    public StyledString getStyledText(Long element, int index, boolean isSelected) {
        return new StyledString("the number " + element);
    }
}