/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model.commands;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Folder;
import org.omnetpp.scave.model.ModelObject;

public class AddChartCommand implements ICommand {

    private Folder container;
    private AnalysisItem item;
    private String originalName;
    private int index;

    public AddChartCommand(Folder container, AnalysisItem item) {
        this(container, item, container.getItems().size());
    }

    public AddChartCommand(Folder container, AnalysisItem item, int index) {
        this.container = container;
        this.item = item;
        this.index = index;
        this.originalName = item.getName();
    }

    private static List<String> getAllChartNames(Folder container) {
        List<String> names = new ArrayList<String>();

        for (AnalysisItem i : container.getItems())
            names.add(i.getName());

        return names;
    }

    private static String makeNameUnique(List<String> existingNames, String name) {
        if (!existingNames.contains(name))
            return name;
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

    @Override
    public void execute() {
        item.setName(makeNameUnique(getAllChartNames(container), item.getName()));
        container.add(item, index);
    }

    @Override
    public void undo() {
        item.setName(originalName);
        container.remove(item);
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
        return Arrays.asList((ModelObject)container, item);
    }

}
