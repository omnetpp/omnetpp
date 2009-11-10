package org.omnetpp.ned.editor.wizards;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.ned.editor.NedEditorPlugin;

/**
 * "New NED file" wizard
 * 
 * @author Andras
 */
public class NEDFileWizard extends AbstractNedFileWizard {

    public NEDFileWizard() {
        setWizardType("nedfile");
    }
    
    @Override
    protected List<IContentTemplate> getTemplates() {
        List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        result.addAll(loadBuiltinTemplates(NedEditorPlugin.getDefault().getBundle()));
        result.addAll(loadTemplatesFromWorkspace(getWizardType()));
        return result;
    }

}
