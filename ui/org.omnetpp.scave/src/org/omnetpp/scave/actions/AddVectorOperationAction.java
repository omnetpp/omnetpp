/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.HashSet;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.commands.SetChartPropertyCommand;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class AddVectorOperationAction extends AbstractScaveAction {
    String operation;
    String comment;

    public AddVectorOperationAction(String name, String operation) {
        this(name, operation, "");
    }

    public AddVectorOperationAction(String name, String operation, String comment) {
        this.operation = operation;
        this.comment = comment;

        setText(name);

        setDescription("Add " + name + " operation to vector");
        setImageDescriptor(ScavePlugin.getImageDescriptor(
                operation.startsWith("compute:") ? ScaveImages.IMG_ETOOL16_COMPUTE : ScaveImages.IMG_ETOOL16_APPLY));
    }


    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) {
        Assert.isTrue(isEnabled());

        ChartScriptEditor scriptEditor = scaveEditor.getActiveChartScriptEditor();

        Chart chart = scriptEditor.getChart();

        Property chartProperty = ScaveModelUtil.getChartProperty(chart, "vector_operations");

        String operations = "";
        if (chartProperty != null)
            operations = chartProperty.getValue();

        String append = operation;
        if (!comment.isEmpty())
            append += " # " + comment;

        if (operations.isEmpty())
            operations = append;
        else
            operations += "\n" + append;

        scriptEditor.getCommandStack().execute(new SetChartPropertyCommand(chart, "vector_operations", operations));

        scriptEditor.refreshChart();
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        if (editor.getActiveChartScriptEditor() == null)
            return false;

        Chart chart = editor.getActiveChartScriptEditor().getChart();
        Property operationsProperty = ScaveModelUtil.getChartProperty(chart, "vector_operations");

        if (operationsProperty != null)
            return true;

        ChartTemplate template = editor.getChartTemplateRegistry().findTemplateByID(chart.getTemplateID());
        if (template == null)
            return false;

        return template.getPropertyNames().contains("vector_operations");
    }
}
