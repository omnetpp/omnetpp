package com.simulcraft.test.gui.recorder.event;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.util.StringUtils;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class MenuSelectionRecognizer extends EventRecognizer {
    public MenuSelectionRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        if (e.type == SWT.MenuDetect) {
            System.out.println("menudetect");
            // note: this is only delivered for context menus, looks like
        }
        if (e.type == SWT.Selection && e.widget instanceof MenuItem) {
            MenuItem menuItem = (MenuItem)e.widget;

            List<String> path = new ArrayList<String>();
            path.add(menuItem.getText());
            MenuItem item = menuItem;
            while (item.getParent().getParentItem() != null) {
                path.add(0, item.getParent().getParentItem().getText());
                item = item.getParent().getParentItem();
            }

            String menuPath = StringUtils.join(path, "|");
            Menu rootMenu = item.getParent();

            Display display = Display.getCurrent();
            if (display.getFocusControl().getMenu() == rootMenu)
                return makeSeq(display.getFocusControl(), expr("chooseFromContextMenu("+quote(menuPath)+")", 0.8, null));
            else if (display.getActiveShell().getMenuBar() == rootMenu)
                return makeSeq(display.getActiveShell(), expr("chooseFromMainMenu("+quote(menuPath)+")", 0.8, null));
            // try harder? like: menuOwner = findMenuOwner(rootMenu); ...
        }
        return null;
    }

    // currently unused
    protected Control findMenuOwner(Menu menu) {
        for (Shell shell : Display.getCurrent().getShells()) {
            if (shell.getMenuBar() == menu)
                return shell;
            Control owner = doFindMenuOwner(shell, menu);
            if (owner != null) 
                return owner;
        }
        return null;
    }

    // currently unused
    protected Control doFindMenuOwner(Control control, Menu menu) {
        if (control.getMenu() == menu)
            return control;
        if (control instanceof Composite) {
            for (Control child : ((Composite)control).getChildren()) {
                Control owner = doFindMenuOwner(child, menu);
                if (owner != null) 
                    return owner;
            }
        }
        return null;
    }
}

