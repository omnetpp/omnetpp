/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.virtualtable.AbstractRowRenderer;
import org.omnetpp.scave.engine.EnumType;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItem.DataType;
import org.omnetpp.scave.engine.VectorDatum;
import org.omnetpp.scave.model2.ResultItemRef;
import org.omnetpp.scave.model2.ResultItemValueFormatter;

/**
 * Implementation of the IVirtualTableItemProvier interface for
 * the table of vector data.
 *
 * @author tomi
 */
public class VectorResultRowRenderer extends AbstractRowRenderer<VectorDatum> {
    private static final int HORIZONTAL_SPACING = 4;

    protected DataType type;
    protected EnumType enumType;
    protected ResultItemValueFormatter valueLabelProvider = new ResultItemValueFormatter();
    protected int fontHeight;

    public void setInput(Object input) {
        if (input instanceof ResultItemRef)
            valueLabelProvider.setResultItem((ResultItemRef)input);
        else
            valueLabelProvider.setResultItem((ResultItem)null);
    }

    public String getTooltipText(VectorDatum element) {
        return null;
    }

    @Override
    public int getIndentation(VectorDatum element, int index) {
        return HORIZONTAL_SPACING;
    }

    @Override
    public Image getImage(VectorDatum element, int index) {
        return null;
    }

    @Override
    public StyledString getStyledText(VectorDatum element, int index, boolean isSelected) {
        String text = null;
        switch (index) {
        case 0:
            text = String.valueOf(element.getSerial());
            break;
        case 1:
            BigDecimal time = element.getSimtime();
            text = (time != null ? time.toString() : "");
            break;
        case 2:
            text = valueLabelProvider.format(element.getValue());
            break;
        case 3:
            text = String.valueOf(element.getEventNumber());
            break;
        default:
            Assert.isTrue(false);
        }
        return new StyledString(text);
    }
}
