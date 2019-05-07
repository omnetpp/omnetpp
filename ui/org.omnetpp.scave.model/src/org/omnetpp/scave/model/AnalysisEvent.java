package org.omnetpp.scave.model;

public class AnalysisEvent {
    private AnalysisObject subject;

    public AnalysisEvent(AnalysisObject subject) {
        this.subject = subject;
    }

    public AnalysisObject getSubject() {
        return subject;
    }
}
