package org.omnetpp.sequencechart.editors;

import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.NavigationLocation;
import org.omnetpp.common.util.BigDecimal;

public class SequenceChartLocation extends NavigationLocation {
    // TODO: ambiguous when restored
    private BigDecimal startSimulationTime;
    private BigDecimal endSimulationTime;
    private String text;

    public SequenceChartLocation(SequenceChartEditor sequenceChartEditor, BigDecimal startSimulationTime, BigDecimal endSimulationTime) {
        super(sequenceChartEditor);
        this.startSimulationTime = startSimulationTime;
        this.endSimulationTime = endSimulationTime;
        this.text = sequenceChartEditor.getTitle() + ": " + startSimulationTime + "s - " + endSimulationTime + "s";
    }

    @Override
    public String getText() {
        return text;
    }

    public boolean mergeInto(INavigationLocation currentLocation) {
        return equals(currentLocation);
    }

    public void restoreLocation() {
        IEditorPart editorPart = getEditorPart();
        if (startSimulationTime != null && endSimulationTime != null && editorPart instanceof SequenceChartEditor) {
            SequenceChartEditor sequenceChartEditor = (SequenceChartEditor)editorPart;
            sequenceChartEditor.getSequenceChart().zoomToSimulationTimeRange(startSimulationTime, endSimulationTime);
        }
    }

    public void restoreState(IMemento memento) {
        String value = memento.getString("startSimulationTime");
        if (value != null)
            startSimulationTime = new BigDecimal(value);
        value = memento.getString("endSimulationTime");
        if (value != null)
            endSimulationTime = new BigDecimal(value);
    }

    public void saveState(IMemento memento) {
        memento.putString("startSimulationTime", startSimulationTime.toString());
        memento.putString("endSimulationTime", endSimulationTime.toString());
    }

    public void update() {
        // void
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((endSimulationTime == null) ? 0 : endSimulationTime.hashCode());
        result = prime * result + ((startSimulationTime == null) ? 0 : startSimulationTime.hashCode());
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        SequenceChartLocation other = (SequenceChartLocation) obj;
        if (endSimulationTime == null) {
            if (other.endSimulationTime != null)
                return false;
        }
        else if (!endSimulationTime.equals(other.endSimulationTime))
            return false;
        if (startSimulationTime == null) {
            if (other.startSimulationTime != null)
                return false;
        }
        else if (!startSimulationTime.equals(other.startSimulationTime))
            return false;
        return true;
    }
}