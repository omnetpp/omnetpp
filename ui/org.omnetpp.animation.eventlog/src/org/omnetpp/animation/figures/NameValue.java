package org.omnetpp.animation.figures;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.FlowLayout;
import org.eclipse.draw2d.Label;

public class NameValue extends Figure {
    private Label name;

    private Label value;

    public NameValue(String name) {
        this(name, "");
    }

    public NameValue(String name, String value) {
        this.name = new Label(name + ":");
        this.value = new Label(value);
        setLayoutManager(new FlowLayout());
        add(this.name);
        add(this.value);
    }

    public void setName(String name) {
        this.name.setText(name);
    }

    public void setValue(double value) {
        this.value.setText(String.valueOf(value));
    }
}
