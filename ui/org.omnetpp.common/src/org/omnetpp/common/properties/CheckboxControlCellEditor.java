/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.properties;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.TraverseEvent;
import org.eclipse.swt.events.TraverseListener;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

/**
 * A boolean property editor using an SWT checkbox as the editor.
 * @author rhornig
 */
public class CheckboxControlCellEditor extends CellEditor {

    Button chkBtn;

    public CheckboxControlCellEditor(Composite parent) {
        super(parent);
    }

    @Override
    protected Control createControl(Composite parent) {
        // KLUDGE: workaround bug in Windows 7, checkbox background is not filled and remains transparent
        Composite hack = null;
        boolean isWindows = SWT.getPlatform().equals("win32");
        if (isWindows) {
            hack = new Composite(parent, SWT.NONE);
            hack.setLayout(new FillLayout());
            parent = hack;
        }
        chkBtn = new Button(parent, SWT.CHECK);

        // see also ComboBoxCellEditor, TextCellEditor etc for explanation
        chkBtn.addKeyListener(new KeyAdapter() {
            // hook key pressed - see PR 14201
            public void keyPressed(KeyEvent e) {
                keyReleaseOccured(e);
                // note: as a result of processing the above call, clients may have
                // disposed this cell editor
            }
        });
        chkBtn.addTraverseListener(new TraverseListener() {
            public void keyTraversed(TraverseEvent e) {
                if (e.detail == SWT.TRAVERSE_ESCAPE || e.detail == SWT.TRAVERSE_RETURN) {
                    e.doit = false;
                }
            }
        });
        return isWindows ? hack : chkBtn;
    }

    @Override
    protected Object doGetValue() {
        return chkBtn.getSelection();
    }

    @Override
    protected void doSetFocus() {
        chkBtn.setFocus();
    }

    @Override
    protected void doSetValue(Object value) {
        Assert.isTrue(value != null && (value instanceof Boolean));
        chkBtn.setSelection(((Boolean)value));
    }
}
