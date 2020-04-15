package com.swtworkbench.community.xswt.test;

import java.io.IOException;

import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;

public class Snippet {
    public void createPartControl(Shell xswtParent) {
        Label label1 = new Label(xswtParent, 0);
        label1.setText("Java Editor settings:");
        TabFolder tabFolder1 = new TabFolder(xswtParent, 0);
        GridData gridData1 = new GridData();
        gridData1.grabExcessHorizontalSpace = true;
        gridData1.grabExcessVerticalSpace = true;
        gridData1.horizontalAlignment = 4;
        gridData1.verticalAlignment = 4;
        tabFolder1.setLayoutData(gridData1);
        Composite appearance = new Composite(tabFolder1, 0);
        GridData gridData2 = new GridData();
        gridData2.grabExcessHorizontalSpace = true;
        gridData2.horizontalAlignment = 4;
        appearance.setLayoutData(gridData2);
        GridLayout gridLayout1 = new GridLayout();
        gridLayout1.numColumns = 2;
        appearance.setLayout(gridLayout1);
        List list1 = new List(appearance, 2052);
        list1.setSelection(new String[] { "0" });
        GridData gridData3 = new GridData();
        gridData3.heightHint = 100;
        gridData3.widthHint = 220;
        list1.setLayoutData(gridData3);
        String string1 = new String("Line number foreground");
        list1.add(string1);
        String string2 = new String("Matching brackets highlight");
        list1.add(string2);
        String string3 = new String("Current line highlight");
        list1.add(string3);
        TabItem tabItem1 = new TabItem(tabFolder1, 0);
        tabItem1.setText("Appeara&nce");
        tabItem1.setControl(appearance);
        Composite syntax = new Composite(tabFolder1, 0);
        GridData gridData4 = new GridData();
        gridData4.grabExcessHorizontalSpace = true;
        gridData4.horizontalAlignment = 4;
        gridData4.horizontalSpan = 2;
        syntax.setLayoutData(gridData4);
        GridLayout gridLayout2 = new GridLayout();
        gridLayout2.numColumns = 2;
        syntax.setLayout(gridLayout2);
        Label label2 = new Label(syntax, 0);
        label2.setText("Fo&reground:");
        GridData gridData5 = new GridData();
        gridData5.horizontalSpan = 2;
        label2.setLayoutData(gridData5);
        TabItem tabItem2 = new TabItem(tabFolder1, 0);
        tabItem2.setText("Synta&x");
        tabItem2.setControl(syntax);
    }

    public static void main(String[] args) throws IOException {
        Display display = new Display();
        Shell shell = new Shell(display);
        shell.setLayout(new GridLayout());
        new Snippet().createPartControl(shell);
        shell.setSize(600, 600);
        shell.open();
        while (!(shell.isDisposed())) {
            if (!(display.readAndDispatch()))
                display.sleep();
        }
        display.dispose();
    }
}