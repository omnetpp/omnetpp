package org.omnetpp.figures.misc;

import org.eclipse.draw2d.Label;
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
    protected boolean centerAlign = false;

    /**
     * Creates a new LabelCellEditorLocator for the given Label
     * @param label
     */
    public LabelCellEditorLocator(Label label) {
        this.label = label;
    }

    /**
     * Creates a new LabelCellEditorLocator for the given Label
     * @param label
     * @param centerAlign whether to align the center of the cell editor to the center of the label
     */
    public LabelCellEditorLocator(Label label, boolean centerAlign) {
        this.label = label;
        this.centerAlign = centerAlign;
    }

    /**
     * @see CellEditorLocator#relocate(org.eclipse.jface.viewers.CellEditor)
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
        Rectangle labelBounds = label.getTextBounds().getTranslated(-1,0);
        label.translateToAbsolute(labelBounds);
        if (centerAlign)
            text.setBounds(labelBounds.x + (labelBounds.width- editorSize.x)/2 + 1, labelBounds.y, editorSize.x + 1, editorSize.y + 1);
        else
            text.setBounds(labelBounds.x - 2, labelBounds.y, editorSize.x + 1, editorSize.y + 1);
        System.out.println(labelBounds);
    }

}
