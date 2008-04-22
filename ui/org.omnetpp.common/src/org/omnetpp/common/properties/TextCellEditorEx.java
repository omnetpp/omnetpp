package org.omnetpp.common.properties;

import java.text.MessageFormat;

import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;

/**
 * Adds dialog support for normal text cell editor. Derived classes should implement
 * the openDialogBox method
 * @author rhornig
 */
public abstract class TextCellEditorEx extends TextCellEditor {
    protected Button button;
    private boolean enableApplyOnFocusLost = true;
    protected Text text;
    
    protected FocusListener textFocusListener = new FocusListener() {
        public void focusGained(FocusEvent e) {
        }

        public void focusLost(FocusEvent e) {
            applyValuesOnFocusLost();
        }
    };

    public TextCellEditorEx() {
        super();
    }

    public TextCellEditorEx(Composite parent, int style) {
        super(parent, style);
    }

    public TextCellEditorEx(Composite parent) {
        super(parent);
    }

    @Override
    protected Control createControl(Composite parent) {
        final Composite cell = new Composite(parent, getStyle());
        cell.setFont(parent.getFont());
        cell.setBackground(parent.getBackground());
        
        GridLayout gridLayout = new GridLayout(2,false);
        gridLayout.marginHeight = gridLayout.marginWidth = 0;
        gridLayout.horizontalSpacing = 0;
        
        cell.setLayout(gridLayout);

        text = (Text)super.createControl(cell);
        text.setLayoutData(new GridData(SWT.FILL,SWT.FILL,true,true));
        
        button = new Button(cell, SWT.DOWN);
        button.setLayoutData(new GridData(SWT.BEGINNING,SWT.FILL,false,true));
        button.setText("...");

        // support pressing cancel even if the button has the focus
        button.addKeyListener(new KeyAdapter() {
            public void keyReleased(KeyEvent e) {
                if (e.character == '\u001b') { // Escape
                    fireCancelEditor();
                }
            }
        });
        
        // disable applyOnFocusLoss in case the mouse is over the button
        // if we clicked on the button we should not apply the current value
        button.addMouseTrackListener(new MouseTrackAdapter() {
            @Override
            public void mouseEnter(MouseEvent e) {
                enableApplyOnFocusLost = false;
            }
            
            @Override
            public void mouseExit(MouseEvent e) {
                enableApplyOnFocusLost = true;
            }
        });
        
        button.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent event) {
                // Remove the text's focus listener since it's guaranteed
                // to lose focus when the dialog opens so we will not apply the values
                text.removeFocusListener(textFocusListener);
                Object newValue = openDialogBox(cell);
                // Re-add the listener once the dialog closes
                text.addFocusListener(textFocusListener);

                if (newValue != null) {
                    boolean newValidState = isCorrect(newValue);
                    if (newValidState) {
                        markDirty();
                        doSetValue(newValue);
                    }
                    else {
                        // try to insert the current value into the error message.
                        setErrorMessage(MessageFormat.format(getErrorMessage(),
                                new Object[] { newValue.toString() }));
                    }
                    fireApplyEditorValue();
                }
            }
        });

        return cell;
    }

    @Override
    protected void focusLost() {
        // disable the original focusLost listener inherited from TextCellEditor
        // the new one is called applyValuesOnFocusLost
    }
    
    protected void applyValuesOnFocusLost() {
        if (isActivated() && enableApplyOnFocusLost) {
            fireApplyEditorValue();
            deactivate();
        }
    }
    
    @Override
    protected void doSetValue(Object value) {
        if (value == null )
            value = "";
        super.doSetValue(value);
    }
    
    protected abstract Object openDialogBox(Control cellEditorWindow);
}
