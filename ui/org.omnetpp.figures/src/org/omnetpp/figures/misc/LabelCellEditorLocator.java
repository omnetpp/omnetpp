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
 * @author rhornig
 * Places the cell editor over the provided label (and sets its fontsize too)
 */
public class LabelCellEditorLocator implements CellEditorLocator {
    private Label label;

    /**
     * Creates a new LabelCellEditorLocator for the given Label
     * 
     * @param label
     *            the Label
     */
    public LabelCellEditorLocator(Label label) {
        this.label = label;
    }

    /**
     * @see CellEditorLocator#relocate(org.eclipse.jface.viewers.CellEditor)
     */
    public void relocate(CellEditor celleditor) {
        Text text = (Text)celleditor.getControl();
        // TODO set the font size too
        Font scaledFont = label.getFont();
        FontData data = scaledFont.getFontData()[0];
        Dimension fontSize = new Dimension(0, data.getHeight());
        label.translateToAbsolute(fontSize);
        data.setHeight(fontSize.height);
        scaledFont = new Font(null, data);
        text.setFont(scaledFont);

        Point pref = text.computeSize(-1, -1);
        Rectangle rect = label.getTextBounds().getCopy();
        label.translateToAbsolute(rect);
        text.setBounds(rect.x - 1, rect.y - 1, pref.x + 1, pref.y + 1);
}

}
