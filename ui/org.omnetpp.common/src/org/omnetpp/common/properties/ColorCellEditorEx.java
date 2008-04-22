package org.omnetpp.common.properties;

import java.util.List;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.ColorDialog;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.contentassist.ContentProposalProvider;

/**
 * A cell editor that manages a color field. using the ColorFactory
 * Supports content assist function, and direct text editing of the color name
 * <p>
 * This is a copied/modified version of the platform ColorCellEditor
 * </p>
 */
public class ColorCellEditorEx extends TextCellEditorEx {

    /**
     * The image.
     */
    private Image image;

    public static class ColorContentProposalProvider extends ContentProposalProvider {
        public ColorContentProposalProvider() {
            super(true);
        }

        @Override
        protected List<IContentProposal> getProposalCandidates(String prefix) {
            return sort(toProposals(ColorFactory.getColorNames()));
        }
        
    }

    /**
     * Creates a new color cell editor with the given control as parent.
     * The cell editor value is black (<code>RGB(0,0,0)</code>) initially, and has no 
     * validator.
     *
     * @param parent the parent control
     */
    public ColorCellEditorEx(Composite parent) {
        this(parent, SWT.NONE);
    }

    /**
     * Creates a new color cell editor with the given control as parent.
     * The cell editor value is black (<code>RGB(0,0,0)</code>) initially, and has no 
     * validator.
     *
     * @param parent the parent control
     * @param style the style bits
     * @since 2.1
     */
    public ColorCellEditorEx(Composite parent, int style) {
        super(parent, style);
        doSetValue("");
    }

    /**
     * Creates and returns the color image data for the given control
     * and RGB value. The image's size is either the control's item extent 
     * or the cell editor's default extent, which is 16 pixels square.
     *
     * @param w the control
     * @param color the color
     */
//    private ImageData createColorImage(Control w, RGB color) {
//
//        GC gc = new GC(w);
//        FontMetrics fm = gc.getFontMetrics();
//        int size = fm.getAscent();
//        gc.dispose();
//
//        int indent = 6;
//        int extent = DEFAULT_EXTENT;
//        if (w instanceof Table) {
//            extent = ((Table) w).getItemHeight() - 1;
//        } else if (w instanceof Tree) {
//            extent = ((Tree) w).getItemHeight() - 1;
//        } else if (w instanceof TableTree) {
//            extent = ((TableTree) w).getItemHeight() - 1;
//        }
//
//        if (size > extent) {
//            size = extent;
//        }
//
//        int width = indent + size;
//        int height = extent;
//
//        int xoffset = indent;
//        int yoffset = (height - size) / 2;
//
//        RGB black = new RGB(0, 0, 0);
//        PaletteData dataPalette = new PaletteData(new RGB[] { black, black,
//                color });
//        ImageData data = new ImageData(width, height, 4, dataPalette);
//        data.transparentPixel = 0;
//
//        int end = size - 1;
//        for (int y = 0; y < size; y++) {
//            for (int x = 0; x < size; x++) {
//                if (x == 0 || y == 0 || x == end || y == end) {
//                    data.setPixel(x + xoffset, y + yoffset, 1);
//                } else {
//                    data.setPixel(x + xoffset, y + yoffset, 2);
//                }
//            }
//        }
//
//        return data;
//    }

    @Override
    protected Control createControl(Composite parent) {
        Control result = super.createControl(parent);
        IContentProposalProvider proposalProvider = new ColorContentProposalProvider();
        new ContentAssistCommandAdapter(text, new TextContentAdapter(), proposalProvider, 
                ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, null, true);

        return result;
    }

    /* (non-Javadoc)
     * Method declared on CellEditor.
     */
    public void dispose() {
        if (image != null) {
            image.dispose();
            image = null;
        }
        super.dispose();
    }

    /* (non-Javadoc)
     * Method declared on DialogCellEditor.
     */
    protected Object openDialogBox(Control cellEditorWindow) {
        ColorDialog dialog = new ColorDialog(cellEditorWindow.getShell());
        RGB value = ColorFactory.asRGB((String)getValue());
        if (value != null) {
            dialog.setRGB(value);
        }
        value =  dialog.open();
        return (value == null) ? null : ColorFactory.asString(value);
    }

}
