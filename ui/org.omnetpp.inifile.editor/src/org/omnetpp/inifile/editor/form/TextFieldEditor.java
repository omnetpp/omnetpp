package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusAdapter;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.inifile.editor.contentassist.InifileValueContentProposalProvider;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ConfigKey.DataType;

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
//XXX disable Text when value is not editable (comes from included file)?
public class TextFieldEditor extends FieldEditor {
	private static final char[] AUTOACTIVATION_CHARS = null; // "( ".toCharArray();
	private Text textField;
	private Label label;
	private Button resetButton;
	private boolean isEdited;
	private ControlDecoration problemDecoration;

	public TextFieldEditor(Composite parent, ConfigKey entry, IInifileDocument inifile, FormPage formPage, String labelText) {
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
		int width = (entry.getDataType()==DataType.CFG_INT || entry.getDataType()==DataType.CFG_DOUBLE) ? 80 : 250;
		textField.setLayoutData(new GridData(width, SWT.DEFAULT));
		((GridData)textField.getLayoutData()).horizontalIndent = 3; // room for the decoration
		resetButton.setLayoutData(new GridData());

		reread();

		// enable Reset button on editing
		textField.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				if (!isEdited) {
					isEdited = true;
					textField.setForeground(null);
					resetButton.setEnabled(true);
				}
			}
		});
		// commit the editfield on Enter
		textField.addSelectionListener(new SelectionAdapter() {
			public void widgetDefaultSelected(SelectionEvent e) {
				commit();
			}
		});

		// commit on losing focus, select all on gaining focus, etc.
		addFocusListenerTo(textField);
		textField.addFocusListener(new FocusAdapter() {
			public void focusGained(FocusEvent e) {
				textField.selectAll();
			}
		});

		// when the background gets clicked, transfer focus to the text widget
		addFocusTransfer(label, textField);
		addFocusTransfer(this, textField);

	}

	protected Text createContentAssistField() {
		Text text = new Text(this, SWT.SINGLE | SWT.BORDER);
        String key = entry.isPerObject() ? "**."+entry.getKey() : entry.getKey();
		InifileValueContentProposalProvider proposalProvider = new InifileValueContentProposalProvider(GENERAL, key, inifile, null, false);
		if (proposalProvider.isContentAssistAvailable()) {
			// only make it a content assist field if proposals are really available
			ContentAssistCommandAdapter commandAdapter = new ContentAssistCommandAdapter(text,
					new TextContentAdapter(), proposalProvider,
					ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, AUTOACTIVATION_CHARS, true);
			commandAdapter.setProposalAcceptanceStyle(ContentProposalAdapter.PROPOSAL_REPLACE);
		}
		return text;
	}

	@Override
	public void reread() {
		// update text and reset button
		String key = entry.isPerObject() ? "**."+entry.getKey() : entry.getKey();
		String value = getValueFromFile(GENERAL, key);
		if (value==null) {
			String defaultValue = entry.getDefaultValue()==null ? "" : entry.getDefaultValue().toString();
			textField.setText(defaultValue);
			//textField.setForeground(ColorFactory.asColor("darkGreen"));
			textField.setForeground(ColorFactory.GREY50);
			resetButton.setEnabled(false);
		}
		else {
			textField.setText(value);
			textField.setForeground(null);
			resetButton.setEnabled(true);
		}
		isEdited = false;

		// update problem decoration
		IMarker[] markers = InifileUtils.getProblemMarkersFor(GENERAL, key, inifile);
		problemDecoration.setImage(getProblemImage(markers, true, true));
		problemDecoration.setDescriptionText(getProblemsText(markers));
		redraw(); // otherwise an obsolete error decoration doesn't disappear
	}

	@Override
	public void commit() {
		if (isEdited) {
			String key = entry.isPerObject() ? "**."+entry.getKey() : entry.getKey();
			String value = textField.getText();
			setValueInFile(GENERAL, key, value);
			isEdited = false;
		}
	}
}
