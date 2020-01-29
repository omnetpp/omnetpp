package org.omnetpp.scave.model.commands;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.DialogPage;
import org.omnetpp.scave.model.ModelObject;

public class ResetChartToTemplateCommand implements ICommand {

    private Chart chart;
    private String oldScript;
    private String newScript;
    private List<DialogPage> oldDialogPages;
    private List<DialogPage> newDialogPages;

    public ResetChartToTemplateCommand(Chart chart, String newScript, List<DialogPage> dialogPages) {
        this.chart = chart;
        this.newScript = newScript;
        this.newDialogPages = dialogPages;
    }

    @Override
    public void execute() {
        oldScript = chart.getScript();
        oldDialogPages = new ArrayList<DialogPage>(chart.getDialogPages());

        chart.setScript(newScript);
        chart.setDialogPages(new ArrayList<DialogPage>(newDialogPages));
    }

    @Override
    public void undo() {
        chart.setScript(oldScript);
        chart.setDialogPages(new ArrayList<DialogPage>(oldDialogPages));
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Reset chart to template";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(chart);
    }

}
