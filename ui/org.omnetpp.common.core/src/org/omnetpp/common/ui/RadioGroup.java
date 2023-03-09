package org.omnetpp.common.ui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.widgets.Button;

/**
 * Radio buttons are often used to select a single value from a fixed set
 * of possible values, yet they are treated in widget APIs as separate
 * widgets, making it cumbersome to fill and read them. This utility
 * class allows one to treat a set of related radio buttons as a single entity,
 * and set/get the value with a single call.
 *
 * @author andras
 */
public class RadioGroup {
    private List<Button> buttons = new ArrayList<>();

    public Button add(Button button, Object value) {
        button.setData(value);
        buttons.add(button);
        return button;
    }

    private void ensureExactlyOneSelected() {
        int numSelected = 0;
        for (Button button : buttons)
            if (button.getSelection())
                numSelected++;
        if (numSelected != 1)
            throw new RuntimeException();
    }

    public void setSelection(Object value) {
        for (Button button : buttons)
            button.setSelection(button.getData().equals(value));
        ensureExactlyOneSelected();
    }

    public Object getSelection() {
        ensureExactlyOneSelected();
        for (Button button : buttons)
            if (button.getSelection())
                return button.getData();
        return null; // never reached
    }
}