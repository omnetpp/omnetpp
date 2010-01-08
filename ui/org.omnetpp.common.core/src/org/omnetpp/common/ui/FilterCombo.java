package org.omnetpp.common.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;

/**
 * Combo that shows the message text in grey (see setMessage()/getMessage() methods) 
 * when it has no focus and no other content.
 * 
 * Note: We don't need an equivalent FilterText class, because the plain SWT Text class
 * already supports this behavior, with the SWT.SEARCH style bit; see setMessage()/ 
 * getMessage methods as well.
 * 
 * @author Andras
 */
public class FilterCombo extends Combo {
    private String message = "type filter";
    private Color messageColor = Display.getCurrent().getSystemColor(SWT.COLOR_DARK_GRAY);
    private Color textColor = super.getForeground();
    private boolean showingMessage = false;
    private boolean alreadyNotifying = false;

    public FilterCombo(Composite parent, int style) {
        super(parent, style);

        addFocusListener(new FocusListener() {
            public void focusLost(FocusEvent e) {
                FilterCombo.this.focusLost(e);
            }
            
            public void focusGained(FocusEvent e) {
                FilterCombo.this.focusGained(e);
            }
        });
        
        addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                FilterCombo.this.textChanged(e);
            }
        });
        
        if (!isFocusControl()) {
            setText(message);
            internalSetForeground(messageColor);
            showingMessage = true;
        }
    }

    @Override
    protected void checkSubclass() {
        // allow subclassing
    }

    protected void focusLost(FocusEvent e) {
        if (getText().length()==0) {
            setText(message);
            internalSetForeground(messageColor);
            showingMessage = true;
        }
    }
    
    protected void focusGained(FocusEvent e) {
        if (showingMessage) {
            setText("");
            internalSetForeground(textColor);
            showingMessage = false;
        }
    }

    protected void textChanged(ModifyEvent e) {
        if (super.getText().length()==0 && !isFocusControl() && !alreadyNotifying) {
            alreadyNotifying = true;
            setText(message);
            alreadyNotifying = false;
            internalSetForeground(messageColor);
            showingMessage = true;
        }
        else if (showingMessage) {
            internalSetForeground(textColor);
            showingMessage = false;
        }
    }

    @Override
    public String getText() {
        if (showingMessage)
            return "";
        else
            return super.getText();
    }
    
    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public void setMessageColor(Color messageColor) {
        this.messageColor = messageColor;
        if (showingMessage)
            internalSetForeground(messageColor);
    }

    public Color getMessageColor() {
        return messageColor;
    }
    
    protected void internalSetForeground(Color color) {
        super.setForeground(color);
    }

    @Override
    public void setForeground(Color color) {
        textColor = color;
        if (!showingMessage)
            internalSetForeground(color);
    }
    
    @Override
    public Color getForeground() {
        return textColor;
    }

}
