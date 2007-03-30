/*******************************************************************************
 * Copyright (c) 2005, 2006 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.common.ui;

import org.eclipse.jface.contentassist.SubjectControlContentAssistant;
import org.eclipse.jface.util.Assert;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusAdapter;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.TraverseEvent;
import org.eclipse.swt.events.TraverseListener;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Text;

/**
 * <i>Note: This class is a copy of <code>org.eclipse.jdt.internal.ui.dialogs.TableTextCellEditor</code>,
 * with one change: it does not apply the value on every keystroke. (See comment marked with [Andras] 
 * in the code.) Original class comment follows.</i>
 * 
 * <p>
 * <code>TableTextCellEditor</code> is a copy of TextCellEditor, with the
 * following changes:
 * 
 * <ul>
 * <li> modify events are sent out as the text is changed, and not only after
 * editing is done <i>(this feature got disabled in this copy --Andras)</i></li>
 * 
 * <li>a content assistant is supported</li>
 * 
 * <li>the <code>Control</code> from <code>getControl(Composite)</code>
 * does not notify registered FocusListeners. This is a workaround for bug
 * 58777. </li>
 * 
 * <li>the user can go to the next/previous row with up and down keys</li>
 * </ul>
 */
public class TableTextCellEditor extends CellEditor {
	public interface IActivationListener {
		public void activate();
	}
	
	private final TableViewer fTableViewer;
	private final int fColumn;
	private final String fProperty;
	/**
	 * The editor's value on activation. This value is reset to the
	 * cell when the editor is left via ESC key.
	 */
	String fOriginalValue;
	SubjectControlContentAssistant fContentAssistant;
	private IActivationListener fActivationListener;
	
    protected Text text;

    private boolean isSelection = false;
    private boolean isDeleteable = false;
    private boolean isSelectable = false;

    private static final int defaultStyle = SWT.SINGLE;
	private ModifyListener fModifyListener;
	
	public TableTextCellEditor(TableViewer tableViewer, int column) {
		super(tableViewer.getTable(), defaultStyle);
		fTableViewer= tableViewer;
		fColumn= column;
		fProperty= (String) tableViewer.getColumnProperties()[column];
	}
	
	public void activate() {
		super.activate();
		if (fActivationListener != null)
			fActivationListener.activate();
		fOriginalValue= text.getText();
	}
	
	private void fireModifyEvent(Object newValue) {
		fTableViewer.getCellModifier().modify(
				((IStructuredSelection) fTableViewer.getSelection()).getFirstElement(),
				fProperty, newValue);
	}
	
	protected void focusLost() {
		if (fContentAssistant != null && fContentAssistant.hasProposalPopupFocus()) {
			// skip focus lost if it went to the content assist popup
		} else {
			super.focusLost();
		}
	}
	
	public void setContentAssistant(SubjectControlContentAssistant assistant) {
		fContentAssistant= assistant;
	}
	
	public void setActivationListener(IActivationListener listener) {
		fActivationListener= listener;
	}
	
	public Text getText() {
		return text;
	}
	
    protected void checkDeleteable() {
        boolean oldIsDeleteable = isDeleteable;
        isDeleteable = isDeleteEnabled();
        if (oldIsDeleteable != isDeleteable) {
            fireEnablementChanged(DELETE);
        }
    }

    protected void checkSelectable() {
        boolean oldIsSelectable = isSelectable;
        isSelectable = isSelectAllEnabled();
        if (oldIsSelectable != isSelectable) {
            fireEnablementChanged(SELECT_ALL);
        }
    }

    protected void checkSelection() {
        boolean oldIsSelection = isSelection;
        isSelection = text.getSelectionCount() > 0;
        if (oldIsSelection != isSelection) {
            fireEnablementChanged(COPY);
            fireEnablementChanged(CUT);
        }
    }

	private ModifyListener getModifyListener() {
	    if (fModifyListener == null) {
	        fModifyListener = new ModifyListener() {
	            public void modifyText(ModifyEvent e) {
	                editOccured(e);
	            }
	        };
	    }
	    return fModifyListener;
	}

    /* (non-Javadoc)
     * Method declared on CellEditor.
     */
    protected Control createControl(Composite parent) {
		//workaround for bug 58777: don't accept focus listeners on the text control
		final Control[] textControl= new Control[1];
		Composite result= new Composite(parent, SWT.NONE) {
			public void addListener(int eventType, final Listener listener) {
				if (eventType != SWT.FocusIn && eventType != SWT.FocusOut) {
					textControl[0].addListener(eventType, listener);
				}
			}
		};
        result.setFont(parent.getFont());
        result.setBackground(parent.getBackground());
		result.setLayout(new FillLayout());
		
        text = new Text(result, getStyle());
		textControl[0]= text;
        text.addSelectionListener(new SelectionAdapter() {
            public void widgetDefaultSelected(SelectionEvent e) {
                handleDefaultSelection(e);
            }
        });
		text.addKeyListener(new KeyAdapter() {
			public void keyPressed(KeyEvent e) {
				// support switching rows while editing:
				if (e.stateMask == SWT.MOD1 || e.stateMask == SWT.MOD2) {
					if (e.keyCode == SWT.ARROW_UP || e.keyCode == SWT.ARROW_DOWN) {
					    // allow starting multi-selection even if in edit mode
						deactivate();
						e.doit= false;
						return;
					}
				}
				
				if (e.stateMask != SWT.NONE)
					return;
				
				switch (e.keyCode) {
				case SWT.ARROW_DOWN :
					e.doit= false;
					int nextRow= fTableViewer.getTable().getSelectionIndex() + 1;
					if (nextRow >= fTableViewer.getTable().getItemCount())
						break;
					editRow(nextRow);
					break;
					
				case SWT.ARROW_UP :
					e.doit= false;
					int prevRow= fTableViewer.getTable().getSelectionIndex() - 1;
					if (prevRow < 0)
						break;
					editRow(prevRow);
					break;
					
				case SWT.F2 :
					e.doit= false;
					deactivate();
					break;
				}
			}

			private void editRow(int row) {
				fTableViewer.getTable().setSelection(row);
				IStructuredSelection newSelection= (IStructuredSelection) fTableViewer.getSelection();
				if (newSelection.size() == 1)
					fTableViewer.editElement(newSelection.getFirstElement(), fColumn);
			}
		});
        text.addKeyListener(new KeyAdapter() {
            // hook key pressed - see PR 14201  
            public void keyPressed(KeyEvent e) {
                keyReleaseOccured(e);

                // as a result of processing the above call, clients may have
                // disposed this cell editor
                if ((getControl() == null) || getControl().isDisposed())
                    return;
                checkSelection(); // see explaination below
                checkDeleteable();
                checkSelectable();
            }
        });
        text.addTraverseListener(new TraverseListener() {
            public void keyTraversed(TraverseEvent e) {
                if (e.detail == SWT.TRAVERSE_ESCAPE
                        || e.detail == SWT.TRAVERSE_RETURN) {
                    e.doit = false;
                }
            }
        });
        // We really want a selection listener but it is not supported so we
        // use a key listener and a mouse listener to know when selection changes
        // may have occured
        text.addMouseListener(new MouseAdapter() {
            public void mouseUp(MouseEvent e) {
                checkSelection();
                checkDeleteable();
                checkSelectable();
            }
        });
        text.addFocusListener(new FocusAdapter() {
            public void focusLost(FocusEvent e) {
                TableTextCellEditor.this.focusLost();
            }
        });
        text.setFont(parent.getFont());
        text.setBackground(parent.getBackground());
        text.setText("");//$NON-NLS-1$
        text.addModifyListener(getModifyListener());
		
		return result;
    }

    protected void fireCancelEditor() {
		/* bug 58540: change signature refactoring interaction: validate as you type [refactoring] */
    	text.setText(fOriginalValue);
		super.fireApplyEditorValue();
    }
		
    /**
     * The <code>TextCellEditor</code> implementation of
     * this <code>CellEditor</code> framework method returns
     * the text string.
     *
     * @return the text string
     */
    protected Object doGetValue() {
        return text.getText();
    }

    protected void doSetFocus() {
        if (text != null) {
            text.selectAll();
            text.setFocus();
            checkSelection();
            checkDeleteable();
            checkSelectable();
        }
    }

    /**
     * The <code>TextCellEditor2</code> implementation of
     * this <code>CellEditor</code> framework method accepts
     * a text string (type <code>String</code>).
     *
     * @param value a text string (type <code>String</code>)
     */
    protected void doSetValue(Object value) {
        Assert.isTrue(text != null && (value instanceof String));
        text.removeModifyListener(getModifyListener());
        text.setText((String) value);
        text.addModifyListener(getModifyListener());
    }

    /**
     * Processes a modify event that occurred in this text cell editor.
     * This framework method performs validation and sets the error message
     * accordingly, and then reports a change via <code>fireEditorValueChanged</code>.
     * Subclasses should call this method at appropriate times. Subclasses
     * may extend or reimplement.
     *
     * @param e the SWT modify event
     */
    protected void editOccured(ModifyEvent e) {
        String value = text.getText();
        boolean oldValidState = isValueValid();
        boolean newValidState = isCorrect(value);
        if (!newValidState) {
            // try to insert the current value into the error message.
            setErrorMessage("Invalid value: "+value);
        }
        valueChanged(oldValidState, newValidState);
		//[Andras] fireModifyEvent(text.getText()); // update model on-the-fly
    }

    public LayoutData getLayoutData() {
        return new LayoutData();
    }

    protected void handleDefaultSelection(SelectionEvent event) {
        // same with enter-key handling code in keyReleaseOccured(e);
        fireApplyEditorValue();
        deactivate();
    }

    public boolean isCopyEnabled() {
        if (text == null || text.isDisposed())
            return false;
        return text.getSelectionCount() > 0;
    }

    public boolean isCutEnabled() {
        if (text == null || text.isDisposed())
            return false;
        return text.getSelectionCount() > 0;
    }

    public boolean isDeleteEnabled() {
        if (text == null || text.isDisposed())
            return false;
        return text.getSelectionCount() > 0
                || text.getCaretPosition() < text.getCharCount();
    }

    public boolean isPasteEnabled() {
        if (text == null || text.isDisposed())
            return false;
        return true;
    }

    public boolean isSelectAllEnabled() {
        if (text == null || text.isDisposed())
            return false;
        return text.getCharCount() > 0;
    }

    protected void keyReleaseOccured(KeyEvent keyEvent) {
        if (keyEvent.character == '\r') { // Return key
            // Enter is handled in handleDefaultSelection.
            // Do not apply the editor value in response to an Enter key event
            // since this can be received from the IME when the intent is -not-
            // to apply the value.  
            // See bug 39074 [CellEditors] [DBCS] canna input mode fires bogus event from Text Control
            //
            // An exception is made for Ctrl+Enter for multi-line texts, since
            // a default selection event is not sent in this case. 
            if (text != null && !text.isDisposed()
                    && (text.getStyle() & SWT.MULTI) != 0) {
                if ((keyEvent.stateMask & SWT.CTRL) != 0) {
                    super.keyReleaseOccured(keyEvent);
                }
            }
            return;
        }
        super.keyReleaseOccured(keyEvent);
    }

    public void performCopy() {
        text.copy();
    }

    public void performCut() {
        text.cut();
        checkSelection();
        checkDeleteable();
        checkSelectable();
    }

    public void performDelete() {
        if (text.getSelectionCount() > 0)
            // remove the contents of the current selection
            text.insert(""); //$NON-NLS-1$
        else {
            // remove the next character
            int pos = text.getCaretPosition();
            if (pos < text.getCharCount()) {
                text.setSelection(pos, pos + 1);
                text.insert(""); //$NON-NLS-1$
            }
        }
        checkSelection();
        checkDeleteable();
        checkSelectable();
    }

    public void performPaste() {
        text.paste();
        checkSelection();
        checkDeleteable();
        checkSelectable();
    }

    public void performSelectAll() {
        text.selectAll();
        checkSelection();
        checkDeleteable();
    }

}
