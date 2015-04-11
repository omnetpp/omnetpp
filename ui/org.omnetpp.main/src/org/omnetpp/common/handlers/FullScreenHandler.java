/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.handlers;

import java.util.ArrayList;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.internal.Workbench;

/**
 * This handler switches between full screen and normal mode.
 * By default, it is bound to Ctrl+Shift+F11 in plugin.xml.
 *
 * In full screen mode editors and views are kept visible, while
 * the following components are hidden:
 *  - the window title, border and decorations
 *  - the menu bar at the top
 *  - all toolbars at the top
 *  - the statusbar at the bottom
 *  - the OS taskbar
 *
 *
 * @author levy
 */
@SuppressWarnings("restriction")
public class FullScreenHandler extends AbstractHandler {
    private Menu menuBar;

    private ArrayList<Control> children = new ArrayList<Control>();

    public FullScreenHandler() {
    }

    @Override
    public Object execute(ExecutionEvent event) throws ExecutionException {
        Shell shell = Workbench.getInstance().getActiveWorkbenchWindow().getShell();
        if (shell.getFullScreen()) {
            // restore all previously hidden components
            shell.setMenuBar(menuBar);
            for (Control child : children)
                child.setVisible(true);
            children.clear();
            shell.setFullScreen(false);
        }
        else {
            menuBar = shell.getMenuBar();
            // hide components
            shell.setMenuBar(null);
            for (Control child : shell.getChildren()) {
                // this is a guess which components need to be hidden
                if (!child.getClass().equals(Canvas.class) && !child.getClass().equals(Composite.class)) {
                    child.setVisible(false);
                    children.add(child);
                }
            }
            shell.setFullScreen(true);
        }
        return null;
    }
}
