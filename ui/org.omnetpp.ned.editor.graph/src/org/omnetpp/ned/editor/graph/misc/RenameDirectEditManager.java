package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.VerifyEvent;
import org.eclipse.swt.events.VerifyListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Text;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;

import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.gef.tools.DirectEditManager;
import org.eclipse.jface.viewers.CellEditor;

/**
 * DirectEditManager for renameable objects
 */
public class RenameDirectEditManager extends DirectEditManager {

    Font scaledFont;
    protected VerifyListener verifyListener;
    protected Label label;

    /**
     * Creates a new RenameDirectEditManager with the given attributes.
     * 
     * @param source
     *            the source EditPart
     * @param editorType
     *            type of editor
     * @param locator
     *            the CellEditorLocator
     */
    public RenameDirectEditManager(GraphicalEditPart source, Class editorType, CellEditorLocator locator, Label label) {
        super(source, editorType, locator);
        this.label = label;
    }

    protected void bringDown() {
        // This method might be re-entered when super.bringDown() is called.
        Font disposeFont = scaledFont;
        scaledFont = null;
        super.bringDown();
        if (disposeFont != null)
            disposeFont.dispose();
    }

    protected void initCellEditor() {
        Text text = (Text) getCellEditor().getControl();
        verifyListener = new VerifyListener() {
            public void verifyText(VerifyEvent event) {
                Text text = (Text) getCellEditor().getControl();
                String oldText = text.getText();
                String leftText = oldText.substring(0, event.start);
                String rightText = oldText.substring(event.end, oldText.length());
                GC gc = new GC(text);
                Point size = gc.textExtent(leftText + event.text + rightText);
                gc.dispose();
                if (size.x != 0)
                    size = text.computeSize(size.x, SWT.DEFAULT);
                getCellEditor().getControl().setSize(size.x, size.y);
            }
        };
        text.addVerifyListener(verifyListener);

        String initialLabelText = label.getText();
        getCellEditor().setValue(initialLabelText);
        IFigure figure = getEditPart().getFigure();
        scaledFont = figure.getFont();
        FontData data = scaledFont.getFontData()[0];
        Dimension fontSize = new Dimension(0, data.getHeight());
        label.translateToAbsolute(fontSize);
        data.setHeight(fontSize.height);
        scaledFont = new Font(null, data);

        text.setFont(scaledFont);
    }

    protected void unhookListeners() {
        super.unhookListeners();
        Text text = (Text) getCellEditor().getControl();
        text.removeVerifyListener(verifyListener);
        verifyListener = null;
    }

    //**********************************************************************************************
    // cellEditorLocator to place the cellEditor over the provided label
    public static class LabelCellEditorLocator implements CellEditorLocator {
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
            Text text = (Text) celleditor.getControl();
            Point pref = text.computeSize(-1, -1);
            Rectangle rect = label.getTextBounds().getCopy();
            label.translateToAbsolute(rect);
            text.setBounds(rect.x - 1, rect.y - 1, pref.x + 1, pref.y + 1);
        }

    }

}
