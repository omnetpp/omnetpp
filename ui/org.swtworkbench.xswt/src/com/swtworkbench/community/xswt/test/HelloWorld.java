package com.swtworkbench.community.xswt.test;

import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;

import com.swtworkbench.community.xswt.XSWT;
import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.layoutbuilder.SWTSourceBuilder;

public class HelloWorld {
    public static void main(String[] args) {
        Display display = new Display();
        Shell shell = new Shell(display, 1264);
        shell.setText("Hello World");
        HelloWorld h = new HelloWorld();
        try {
            SWTSourceBuilder source = new SWTSourceBuilder();
            XSWT.layoutBuilder = source;

            XSWT.create(null, h.getClass().getResourceAsStream("demo.xswt"));

            System.out.println(source);
        }
        catch (XSWTException e) {
            e.printStackTrace();
        }
        shell.setBounds(50, 50, 600, 400);

        shell.open();
        while (!(shell.isDisposed())) {
            if (display.readAndDispatch())
                continue;
            display.sleep();
        }
        display.dispose();
    }
}