package org.omnetpp.scave.model.commands;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.ModelObject;

public class AddChartCommand implements ICommand {

    private Analysis analysis;
    private AnalysisItem chart;
    private String originalName;
    private int index;

    public AddChartCommand(Analysis analysis, AnalysisItem chart) {
        this(analysis, chart, analysis.getCharts().getCharts().size());
    }

    private static List<String> getAllChartNames(Analysis analysis) {
        List<String> names = new ArrayList<String>();

        for (AnalysisItem i : analysis.getCharts().getCharts())
            names.add(i.getName());

        return names;
    }

    private static String makeNameUnique(List<String> existingNames, String name) {
        String nameWithoutNumber = name.replaceAll(" \\(\\d+\\)$", "");
        String pattern = "^" + Pattern.quote(nameWithoutNumber) + " \\((\\d+)\\)$";

        int maxNum = -1;
        for (String existing : existingNames) {
            int num = -1;

            if (existing.equals(nameWithoutNumber))
                num = 0;

            Matcher matcher = Pattern.compile(pattern).matcher(existing);
            if (matcher.matches())
                num = Integer.parseInt(matcher.group(1));

            if (num > maxNum)
                maxNum = num;
        }

        return nameWithoutNumber + (maxNum == -1 ? "" : maxNum == 0 ? " (2)" : (" (" + (maxNum + 1) + ")"));
    }

    public AddChartCommand(Analysis analysis, AnalysisItem chart, int index) {
        this.analysis = analysis;
        this.chart = chart;
        this.index = index;
        this.originalName = chart.getName();
    }

    @Override
    public void execute() {
        chart.setName(makeNameUnique(getAllChartNames(analysis), chart.getName()));
        analysis.getCharts().addChart(chart, index);
    }

    @Override
    public void undo() {
        chart.setName(originalName);
        analysis.getCharts().removeChart(chart);
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Add chart";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(analysis.getCharts(), chart);
    }

}
