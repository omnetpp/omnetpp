package org.omnetpp.figures.misc;

import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Text;

/**
 * Places the cell editor over the provided label (and sets its font size too)
 *
 * @author rhornig
 */
public class LabelCellEditorLocator implements CellEditorLocator {
    protected Label label;

    /**
     * Creates a new LabelCellEditorLocator for the given Label
     * @param label
     */
    public LabelCellEditorLocator(Label label) {
        this.label = label;
    }

    /**
     * @see CellEditorLocator#relocate(org.eclipse.jface.viewers.CellEditor)
     * relocates the cell editor taking into account the current label location and alignment
     * (ie. centered labels cell editor will be centered around the labels center)
     * Also sets the font size according to the zoom setting.
     */
    public void relocate(CellEditor celleditor) {
        Text text = (Text)celleditor.getControl();
        Font scaledFont = label.getFont();
        FontData data = scaledFont.getFontData()[0];
        Dimension fontSize = new Dimension(0, data.getHeight());
        label.translateToAbsolute(fontSize);
        data.setHeight(fontSize.height);
        scaledFont = new Font(null, data);
        text.setFont(scaledFont);

        Point editorSize = text.computeSize(-1, -1);
        Rectangle labelBounds = label.getTextBounds();
        label.translateToAbsolute(labelBounds);
        // if the label is center aligned we mimic that with the cell editor too
        if (label.getTextAlignment() == PositionConstants.CENTER)
            text.setBounds(labelBounds.x + (labelBounds.width - editorSize.x)/2 , labelBounds.y, editorSize.x, editorSize.y + 1);
        else
            text.setBounds(labelBounds.x, labelBounds.y, editorSize.x, editorSize.y + 1);
    }

}
