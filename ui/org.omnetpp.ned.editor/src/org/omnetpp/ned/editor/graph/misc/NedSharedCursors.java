package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.eclipse.jface.resource.ImageDescriptor;

import org.eclipse.draw2d.Cursors;

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
        CURSOR_PLUG = createCursor("cursors/plug.gif"); //$NON-NLS-1$
        CURSOR_PLUG_NOT = createCursor("cursors/plug_not.gif"); //$NON-NLS-1$
        CURSOR_TREE_ADD = createCursor("cursors/tree_add.gif"); //$NON-NLS-1$
        CURSOR_TREE_MOVE = createCursor("cursors/tree_move.gif"); //$NON-NLS-1$
    }

    private static Cursor createCursor(String sourceName) {
        Image src = NedEditorPlugin.getImage(sourceName);
        return new Cursor(null, src.getImageData(), 0, 0);
    }
}
