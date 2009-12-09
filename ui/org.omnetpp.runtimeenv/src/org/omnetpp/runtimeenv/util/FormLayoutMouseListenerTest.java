package org.omnetpp.runtimeenv.util;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Shell;

public class FormLayoutMouseListenerTest {
    public static void run(Shell parentShell) {
        Shell parent = new Shell(parentShell, SWT.RESIZE | SWT.DIALOG_TRIM);
        parent.setText("Test");
        parent.setActive();
        parent.setLayout(new FormLayout());
        parent.setBackground(new Color(null, 228, 228, 228));

        for (int i=0; i<5; i++) {
            //BorderedComposite child = new BorderedComposite(parent, SWT.BORDER);
            //Canvas child = new Canvas(parent, SWT.BORDER);
            //Composite child = new Composite(parent, SWT.BORDER);
            //FigureCanvas child = new FigureCanvas(parent, SWT.BORDER);
            //IFigure rootFigure = child.getLightweightSystem().getRootFigure();
            Button child = new Button(parent, SWT.PUSH);
            child.setText("Move/Resize!");

            FormData formData = new FormData();
            formData.height = 100;
            formData.width = 100;
            //formData.left = new FormAttachment(0, 0);
            //formData.top = new FormAttachment(0, 0);  //XXX (coolBar.getParent(), 0);
            //formData.right = new FormAttachment(100, 0);
            //formData.bottom = new FormAttachment(100, 0);
            child.setLayoutData(formData);

            FormLayoutMouseListener listener = new FormLayoutMouseListener(parent, true, true);
            child.addMouseListener(listener);
            child.addMouseMoveListener(listener);
            child.addMouseTrackListener(listener);
        }

        parent.setSize(500, 500);
        parent.layout(true);
        parent.setVisible(true);

        parent.addControlListener(new ControlAdapter() {
            @Override
            public void controlResized(ControlEvent e) {
                ((Shell)e.widget).layout();
            }
        });
    }
}
