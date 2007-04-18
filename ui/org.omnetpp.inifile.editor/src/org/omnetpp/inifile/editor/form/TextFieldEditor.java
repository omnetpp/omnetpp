package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.jface.fieldassist.TextControlCreator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistField;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.inifile.editor.contentassist.InifileValueContentProposalProvider;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.ConfigurationEntry.DataType;

/**
 * Text-based editor for inifile entries.
 * 
 * NOTE: This class edits the [General] section ONLY. All other sections
 * are ignored. For example, the Reset button only removes the setting
 * from the [Genera] section. When the setting is present outside
 * [General], the table-based field editor has to be used.
 * 
 * @author Andras
 */
public class TextFieldEditor extends FieldEditor {
	private static final char[] AUTOACTIVATION_CHARS = null; // "( ".toCharArray();
	private Text textField;
	private Label label;
	private Button resetButton;
	private boolean isEdited;
	private String section = GENERAL;

	public TextFieldEditor(Composite parent, ConfigurationEntry entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, SWT.NONE, entry, inifile, formPage);

		// layout
		GridLayout gridLayout = new GridLayout(3, false);
		gridLayout.marginTop = gridLayout.marginBottom = gridLayout.marginHeight = gridLayout.verticalSpacing = 0;
		gridLayout.marginHeight = 2;
		setLayout(gridLayout);

		// child widgets
		label = createLabel(entry, labelText+":");
		ContentAssistField contentAssistField = createContentAssistField();
		textField = (Text) contentAssistField.getControl();
		tooltipSupport.adapt(textField);
		resetButton = createResetButton();
		
		// set layout data
		label.setLayoutData(new GridData());
		int width = (entry.getDataType()==DataType.CFG_STRING || entry.getDataType()==DataType.CFG_FILENAME || entry.getDataType()==DataType.CFG_FILENAMES) ? 250 : 80;
		contentAssistField.getLayoutControl().setLayoutData(new GridData(width, SWT.DEFAULT));
		resetButton.setLayoutData(new GridData());

		reread();

		// enable Reset button on editing
		textField.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				if (!isEdited) {
					isEdited = true;
					resetButton.setEnabled(true);
				}
			}
		});
		
		// commit on losing focus, etc.
		addFocusListenerTo(textField);
	}

	protected ContentAssistField createContentAssistField() {
		IContentProposalProvider proposalProvider = new InifileValueContentProposalProvider(GENERAL, entry.getKey(), inifile, null);
		ContentAssistField contentAssistField = 
			new ContentAssistField(this, SWT.SINGLE | SWT.BORDER,
					new TextControlCreator(),
					new TextContentAdapter(),
					proposalProvider,
					ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS,
					AUTOACTIVATION_CHARS);
		return contentAssistField;
	}

	@Override
	public void reread() {
		String value = getValueFromFile(section);
		if (value==null) {
			String defaultValue = entry.getDefaultValue()==null ? "" : entry.getDefaultValue().toString(); 
			textField.setText(defaultValue);
			resetButton.setEnabled(false);
		}
		else {
			textField.setText(value);
			resetButton.setEnabled(true);
		}
		isEdited = false;
	}

	@Override
	public void commit() {
		if (isEdited) {
			String value = textField.getText();
			setValueInFile(section, value);
			isEdited = false;
		}
	}
}
