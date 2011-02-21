package org.omnetpp.inifile.editor.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.ui.texteditor.ResourceAction;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.ParamResolutionTimeoutException;
import org.omnetpp.inifile.editor.model.Timeout;
import org.omnetpp.inifile.editor.text.InifileEditorMessages;
import org.omnetpp.inifile.editor.text.actions.InifileTextEditorAction;

/**
 * Action to enable/disable the long running part of ini file analysis.
 * 
 * @author tomi
 */
public class ToggleAnalysisAction extends ResourceAction implements IPropertyChangeListener {

    public final static String ID = "ToggleAnalysis";
    
    InifileAnalyzer analyzer;

    public ToggleAnalysisAction() {
        super(InifileEditorMessages.getResourceBundle(), ID+".", IAction.AS_CHECK_BOX);
        setId(ID);
        setActionDefinitionId(InifileTextEditorAction.ACTION_DEFINITION_PREFIX+ID);
        setTargetEditor(null);
    }
    
    @Override
    public void run() {
        if (analyzer != null) {
            boolean enabled = !analyzer.isParamResolutionEnabled();
            setChecked(enabled);
            analyzer.setParamResolutionEnabled(enabled);

            if (enabled)
                analyzer.startAnalysisIfChanged();
        }
    }
    
    public void setTargetEditor(InifileEditor editor) {
        if (analyzer != null)
            analyzer.removePropertyChangeListener(this);
        
        analyzer = editor != null ? editor.getEditorData().getInifileAnalyzer() : null;
        
        if (analyzer != null) {
            setChecked(analyzer.isParamResolutionEnabled());
            setEnabled(true);
            analyzer.addPropertyChangeListener(this);
        }
        else {
            setChecked(false);
            setEnabled(false);
        }
    }

    public void propertyChange(PropertyChangeEvent event) {
        if (analyzer == event.getSource() && InifileAnalyzer.PROP_ANALYSIS_ENABLED.equals(event.getProperty()))
            setChecked(analyzer.isParamResolutionEnabled());
    }
}
