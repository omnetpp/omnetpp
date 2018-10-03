/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;

public class ApplyAction extends AbstractScaveAction {
    String codeFragment;
    String marker = "# You can perform any transformations on the data here";

    public ApplyAction(String name, String codeFragment) {
        this.codeFragment = codeFragment;

        setText(name);
        setDescription("Apply " + name + " operation to vector data");

        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_APPLY));
    }


    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        Assert.isTrue(isEnabled());

        ChartScriptEditor scriptEditor = scaveEditor.getActiveChartScriptEditor();

        String script = scriptEditor.getChart().getScript();
        script = script.replace(marker, codeFragment + "\n" + marker);
        scriptEditor.getChart().setScript(script);
        scriptEditor.refreshChart();
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return editor.getActiveChartScriptEditor().getChart().getScript().contains(marker);
    }
}
