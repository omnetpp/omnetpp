package com.swtworkbench.community.xswt.test;

import java.io.IOException;
import java.util.Map;

import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Shell;

import com.swtworkbench.community.xswt.XSWT;
import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.metalogger.Logger;

public class XSWTTest {
    public static void main(String[] args) throws IOException {
        Display display = new Display();
        Shell shell = new Shell(display);
        shell.setLayout(new GridLayout());
        try {
            Map controls = XSWT.create(shell, XSWTTest.class.getResource("bounds_example.xswt").openStream());

            Menu menuBar = (Menu) controls.get("menuBar");
            if (menuBar != null)
                shell.setMenuBar(menuBar);
        }
        catch (XSWTException e) {
            Logger.log().error(e, "Unable to parse XSWT file");
        }
        catch (IOException e) {
            Logger.log().error(e, "Unable to parse XSWT file");
        }

        shell.setSize(600, 600);
        shell.open();
        while (!(shell.isDisposed())) {
            if (!(display.readAndDispatch()))
                display.sleep();
        }
        display.dispose();
    }
}