package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.draw2d.Cursors;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.graphics.Cursor;
import org.omnetpp.ned.editor.NedEditorPlugin;

/**
 * A shared collection of Cursors.
 *
 * @since 2.0
 */
public class NedSharedCursors extends Cursors {

    /**
     * Cursor for valid connection
     */
    public static final Cursor CURSOR_PLUG;
    /**
     * Cursor for invalid connection
     */
    public static final Cursor CURSOR_PLUG_NOT;
    /**
     * Cursor for adding to a tree
     */
    public static final Cursor CURSOR_TREE_ADD;
    /**
     * Cursor for dragging in a tree
     */
    public static final Cursor CURSOR_TREE_MOVE;

    static {
        CURSOR_PLUG = createCursor("cursors/plug.bmp", //$NON-NLS-1$
                "cursors/plugmask.gif"); //$NON-NLS-1$
        CURSOR_PLUG_NOT = createCursor("cursors/plugnot.bmp", //$NON-NLS-1$
                "cursors/plugmasknot.gif"); //$NON-NLS-1$
        CURSOR_TREE_ADD = createCursor("cursors/Tree_Add.gif", //$NON-NLS-1$
                "cursors/Tree_Add_Mask.gif"); //$NON-NLS-1$
        CURSOR_TREE_MOVE = createCursor("cursors/Tree_Move.gif", //$NON-NLS-1$
                "cursors/Tree_Move_Mask.gif"); //$NON-NLS-1$
    }

    private static Cursor createCursor(String sourceName, String maskName) {
        ImageDescriptor src = NedEditorPlugin.getImageDescriptor(sourceName);
        ImageDescriptor mask = NedEditorPlugin.getImageDescriptor(maskName);
        return new Cursor(null, src.getImageData(100), mask.getImageData(100), 0, 0);
    }

}
