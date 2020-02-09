package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.Property;

public class ResetChartToTemplateCommand extends CompoundCommand {

    private Chart chart;

    public ResetChartToTemplateCommand(Chart chart, ChartTemplate template, boolean preserveScript) {
        super("Reset chart to template");
        this.chart = chart;

        append(new SetChartDialogPagesCommand(chart, template.getDialogPages()));
        if (!preserveScript)
            append(new SetChartScriptCommand(chart, template.getPythonScript()));

        for (Property p : chart.getProperties())
            if (!template.getPropertyNames().contains(p.getName()))
                append(new RemoveChartPropertyCommand(chart, p));

        for (String pn : template.getProperties().keySet())
            if (chart.lookupProperty(pn) == null)
                append(new AddChartPropertyCommand(chart, new Property(pn, template.getPropertyDefault(pn))));
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
