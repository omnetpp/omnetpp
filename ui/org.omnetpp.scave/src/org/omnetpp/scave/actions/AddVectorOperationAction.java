/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;

public class AddVectorOperationAction extends AbstractScaveAction {
    String codeFragment;
    String marker = "# <|> vectorops marker <|>";

    public AddVectorOperationAction(String name, String codeFragment) {
        this.codeFragment = codeFragment;

        setText(name);
        setDescription("Apply " + name + " operation to vector data");

        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_APPLY));
    }


    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) {
        Assert.isTrue(isEnabled());

        ChartScriptEditor scriptEditor = scaveEditor.getActiveChartScriptEditor();

        String script = scriptEditor.getDocument().get();
        script = script.replace(marker, codeFragment + "\n" + marker);
        scriptEditor.getDocument().set(script); // TODO only change the inserted portion, without replacing the whole text
        scriptEditor.refreshChart();
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return editor.getActiveChartScriptEditor().getDocument().get().contains(marker);
    }
}
