package org.omnetpp.runtimeenv.util;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.canvas.BorderedComposite;

public class FormLayoutMouseListenerTest {
    public static void run(Shell parentShell) {
        Shell parent = new Shell(parentShell, SWT.RESIZE | SWT.DIALOG_TRIM);
        parent.setText("lofasz");
        parent.setActive();
        parent.setLayout(new FormLayout());
        parent.setBackground(new Color(null, 228, 228, 228));
        
        for (int i=0; i<5; i++) {
            BorderedComposite canvas = new BorderedComposite(parent, SWT.BORDER);
            FormData formData = new FormData();
            formData.height = 100;
            formData.width = 100;
            //formData.left = new FormAttachment(0, 0);
            //formData.top = new FormAttachment(0, 0);  //XXX (coolBar.getParent(), 0);
            //formData.right = new FormAttachment(100, 0);
            //formData.bottom = new FormAttachment(100, 0);
            canvas.setLayoutData(formData);
            
            FormLayoutMouseListener listener = new FormLayoutMouseListener(parent, true, true);
            canvas.addMouseListener(listener);
            canvas.addMouseMoveListener(listener);
            canvas.addMouseTrackListener(listener);
        }
        
        parent.setSize(500, 500);
        parent.layout(true);
        parent.setVisible(true);
    }
}
