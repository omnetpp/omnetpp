package freemarker.eclipse.preferences;

import org.eclipse.jface.preference.BooleanFieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;

import freemarker.eclipse.FreemarkerPlugin;

/**
 * @author Stephan
 *
 * To change this generated comment edit the template variable "typecomment":
 * Window>Preferences>Java>Templates.
 * To enable and disable the creation of type comments go to
 * Window>Preferences>Java>Code Generation.
 */
public class OutlinePreferencePage
    extends FieldEditorPreferencePage
    implements IWorkbenchPreferencePage, IPreferenceConstants {

    public OutlinePreferencePage() {
        super(GRID);
        setPreferenceStore(FreemarkerPlugin.getDefault().getPreferenceStore());
        setDescription("Outline View Settings");
    }

    /**
     * @see org.eclipse.jface.preference.FieldEditorPreferencePage#createFieldEditors()
     */
    protected void createFieldEditors() {
        addField(new BooleanFieldEditor(SHOW_ICONS,
                "Show icons", getFieldEditorParent()));
    }

    /**
     * @see org.eclipse.ui.IWorkbenchPreferencePage#init(IWorkbench)
     */
    public void init(IWorkbench workbench) {
    }

}
