package org.omnetpp.scave.model;

public class ModelChangeEvent {
    private ModelObject subject;

    public ModelChangeEvent(ModelObject subject) {
        this.subject = subject;
    }

    public ModelObject getSubject() {
        return subject;
    }
}
