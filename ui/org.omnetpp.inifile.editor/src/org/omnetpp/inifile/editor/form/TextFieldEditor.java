package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.inifile.editor.contentassist.InifileValueContentProposalProvider;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ConfigurationEntry.DataType;

/**
 * Text-based editor for inifile entries.
 * 
 * NOTE: This class edits the [General] section ONLY. All other sections
 * are ignored. For example, the Reset button only removes the setting
 * from the [General] section. When a setting is present outside
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
	private ControlDecoration problemDecoration;

	public TextFieldEditor(Composite parent, ConfigurationEntry entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, SWT.NONE, entry, inifile, formPage);

		// layout
		GridLayout gridLayout = new GridLayout(3, false);
		gridLayout.marginHeight = 2;
		setLayout(gridLayout);

		// child widgets
		label = createLabel(entry, labelText+":");
		textField = createContentAssistField();
		addTooltipSupport(textField);
		resetButton = createResetButton();

		problemDecoration = new ControlDecoration(textField, SWT.LEFT | SWT.TOP);
		problemDecoration.setShowOnlyOnFocus(false);
		
		// set layout data
		label.setLayoutData(new GridData());
		int width = (entry.getDataType()==DataType.CFG_STRING || entry.getDataType()==DataType.CFG_FILENAME || entry.getDataType()==DataType.CFG_FILENAMES) ? 250 : 80;
		textField.setLayoutData(new GridData(width, SWT.DEFAULT));
		((GridData)textField.getLayoutData()).horizontalIndent = 3; // room for the decoration
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

		addFocusTransfer(label, textField);
		addFocusTransfer(this, textField);

	}

	protected Text createContentAssistField() {
		Text text = new Text(this, SWT.SINGLE | SWT.BORDER);
		IContentProposalProvider proposalProvider = new InifileValueContentProposalProvider(GENERAL, entry.getKey(), inifile, null);
		new ContentAssistCommandAdapter(text, new TextContentAdapter(), proposalProvider, 
				ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, AUTOACTIVATION_CHARS, true);
		return text;
	}

	@Override
	public void reread() {
		// update text and reset button
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

		// update problem decoration
		IMarker[] markers = InifileUtils.getProblemMarkersFor(section, entry.getKey(), inifile);
		problemDecoration.setImage(getProblemImage(markers, true));
		problemDecoration.setDescriptionText(getProblemsText(markers));
		redraw(); // otherwise an obsolete error decoration doesn't disappear 
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
