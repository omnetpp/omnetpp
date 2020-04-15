package org.omnetpp.ned.editor.graph.dialogs;

import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusAdapter;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Spinner;
import org.omnetpp.common.util.UIUtils;

/**
 * Represents a spinner control in the Properties dialog.
 *
 * A challenge is that an SWT Spinner control cannot contain an empty string
 * which is required by IFieldEditor; we try to emulate this feature by setting
 * the text color to the same as the background color.
 *
 * @author Andras
 */
public class SpinnerFieldEditor implements IFieldEditor {
    private Spinner spinner;
    private boolean grayed = false;
    private Color backgroundColor = Display.getCurrent().getSystemColor(SWT.COLOR_LIST_BACKGROUND);
    private ControlDecoration problemDecoration;

    public SpinnerFieldEditor(Composite parent, int style) {
        spinner = new Spinner(parent, style);
        problemDecoration = new ControlDecoration(spinner, SWT.RIGHT | SWT.TOP);
        problemDecoration.setShowOnlyOnFocus(false);

        spinner.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                if (isGrayed())
                    setGrayed(false);
                // Clue: getText()=="" may occur, but only while spinner has focus.
                // Unfortunately, when spinner loses the focus, a non-empty value is written back (see Spinner.wmKillFocus())
                if (spinner.getText().equals("")) {
                    // make text invisible
                    spinner.setForeground(backgroundColor);
                }
                else if (Display.getCurrent().getFocusControl()==spinner && !isLosingFocus()) {
                    // remove grey or backgroundColor, unless we are called internally because Spinner has lost or is losing focus
                    // note: on Linux we already lost the focus when by the time this method gets called internally due to focus loss
                    spinner.setForeground(null);
                }
            }

            private boolean isLosingFocus() {
                // Hack: we are losing focus if Spinner's wmKillFocus() is on the stack (Windows-specific)
                // or when textDidEndEditing() is on the stack (OS X)
                Exception e = new Exception();
                e.fillInStackTrace();
                for (StackTraceElement frame : e.getStackTrace()) {
                    if (frame.getClassName().endsWith(".Spinner") &&
                            (frame.getMethodName().startsWith("wmKillFocus") ||       // Windows
                             frame.getMethodName().startsWith("textDidEndEditing")))  // OS X
                        return true;
                }
                return false;
            }
        });

        // cosmetic hack: prevent "hidden" value to be seen when spinner contents is auto-selected when it gains focus
        spinner.addFocusListener(new FocusAdapter() {
            public void focusGained(FocusEvent e) {
                if (backgroundColor.equals(spinner.getForeground())) {
                    // when we gain focus and text is "hidden", delete the contents of the spinner widget
                    postEvent(SWT.KeyDown, SWT.DEL);
                    postEvent(SWT.KeyUp, SWT.DEL);
                }
            }

            private void postEvent(int type, char character) {
                Event event = new Event();
                event.type = type;
                event.display = Display.getCurrent();
                event.character = character;
                event.keyCode = 0;
                Display.getCurrent().post(event);
            }
        });
    }

    public Spinner getControl() {
        return spinner;
    }

    public boolean isGrayed() {
        return grayed;
    }

    public void setGrayed(boolean grayed) {
        this.grayed = grayed;
        if (grayed) {
            spinner.setBackground(GREY);
            spinner.setForeground(GREY);
        }
        else {
            spinner.setBackground(null); // default system color
            spinner.setForeground(null);
        }
    }

    public String getText() {
        return backgroundColor.equals(spinner.getForeground()) ? "" : spinner.getText();
    }

    public void setText(String text) {
        if (text.equals("")) {
            // spinner cannot display empty string, so we fake it by making the text unreadable
            spinner.setForeground(backgroundColor);
            return;
        }
        boolean ok = false;
        if (text.matches("[0-9]+")) {
            int value = Integer.valueOf(text);
            if (value >= spinner.getMinimum() && value <= spinner.getMaximum()) {
                spinner.setSelection(value);
                ok = true;
            }
        }
        if (!ok)
            setGrayed(true); // cannot display non-numeric and out-of-range values
    }

    public boolean isEnabled() {
        return spinner.isEnabled();
    }

    public void setEnabled(boolean enabled) {
        if (spinner.isEnabled() != enabled)
            spinner.setEnabled(enabled);
    }

    public void setMessage(int severity, String text) {
        UIUtils.updateProblemDecoration(problemDecoration, severity, text);
    }

    public void addModifyListener(ModifyListener listener) {
        spinner.addModifyListener(listener);
    }

    public void removeModifyListener(ModifyListener listener) {
        spinner.removeModifyListener(listener);
    }
}