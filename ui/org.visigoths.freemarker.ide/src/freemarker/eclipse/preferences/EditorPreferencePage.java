package freemarker.eclipse.preferences;

import org.eclipse.jface.preference.BooleanFieldEditor;
import org.eclipse.jface.preference.ColorFieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;

import freemarker.eclipse.FreemarkerPlugin;

/**
 * This class represents a preference page that
 * is contributed to the Preferences dialog. By
 * subclassing <samp>FieldEditorPreferencePage</samp>, we
 * can use the field support built into JFace that allows
 * us to create a page that is small and knows how to
 * save, restore and apply itself.
 * <p>
 * This page is used to modify preferences only. They
 * are stored in the preference store that belongs to
 * the main plug-in class. That way, preferences can
 * be accessed directly via the preference store.
 */

public class EditorPreferencePage
    extends FieldEditorPreferencePage
    implements IWorkbenchPreferencePage, IPreferenceConstants {

    public EditorPreferencePage() {
        super(GRID);
        setPreferenceStore(FreemarkerPlugin.getDefault().getPreferenceStore());
        setDescription("Highlighting Settings");
    }

/**
 * Creates the field editors. Field editors are abstractions of
 * the common GUI blocks needed to manipulate various types
 * of preferences. Each field editor knows how to save and
 * restore itself.
 */

    public void createFieldEditors() {

        addField(new ColorFieldEditor(COLOR_DIRECTIVE,
                "Directive:", getFieldEditorParent()));
        addField(new ColorFieldEditor(COLOR_INTERPOLATION,
                "Interpolation:", getFieldEditorParent()));
        addField(new ColorFieldEditor(COLOR_TEXT,
                "Text:", getFieldEditorParent()));
        addField(new ColorFieldEditor(COLOR_COMMENT,
                "Comment:", getFieldEditorParent()));
        addField(new ColorFieldEditor(COLOR_STRING,
                "String:", getFieldEditorParent()));
        addField(new BooleanFieldEditor(XML_HIGHLIGHTING,
                "HTML/XML Highlighting", getFieldEditorParent()));
        addField(new ColorFieldEditor(COLOR_XML_TAG,
                "HTML/XML Tag:", getFieldEditorParent()));
        addField(new ColorFieldEditor(COLOR_XML_COMMENT,
                "HTML/XML Comment:", getFieldEditorParent()));
    }

    public void init(IWorkbench workbench) {
    }
}