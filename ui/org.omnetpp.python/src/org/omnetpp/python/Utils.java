package org.omnetpp.python;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.handlers.IHandlerService;

public class Utils {
    public static void simulateKeyPress(int keyCode, int stateMask) {
        Display.getDefault().asyncExec(() -> simulateKeyPressInUiThread(keyCode, stateMask));
    }

    public static void simulateKeyPressInUiThread(int keyCode, int stateMask) {
        Shell shell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
        if (shell != null) {
            Event keyDown = new Event();
            keyDown.type = SWT.KeyDown;
            keyDown.keyCode = keyCode;
            keyDown.stateMask = stateMask;
            shell.notifyListeners(SWT.KeyDown, keyDown);
            Event keyUp = new Event();
            keyUp.type = SWT.KeyUp;
            keyUp.keyCode = keyCode;
            keyUp.stateMask = stateMask;
            shell.notifyListeners(SWT.KeyUp, keyUp);
        }
    }

    public static void executeActivateEditorAction() {
        IHandlerService handlerService = (IHandlerService) PlatformUI.getWorkbench().getService(IHandlerService.class);
        try {
            Display.getDefault().asyncExec(() -> {
                try {
                    handlerService.executeCommand("org.eclipse.ui.window.activateEditor", null);
                }
                catch (Exception e) {
                    throw new RuntimeException(e);
                }
            });
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void executeToggleMaximizeViewAction() {
        IHandlerService handlerService = (IHandlerService) PlatformUI.getWorkbench().getService(IHandlerService.class);
        try {
            Display.getDefault().asyncExec(() -> {
                try {
                    handlerService.executeCommand("org.eclipse.ui.window.maximizePart", null);
                }
                catch (Exception e) {
                    throw new RuntimeException(e);
                }
            });
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }
}

