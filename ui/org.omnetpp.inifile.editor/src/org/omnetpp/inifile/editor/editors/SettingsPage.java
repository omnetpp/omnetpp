package org.omnetpp.inifile.editor.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Table;
import org.eclipse.ui.forms.widgets.ExpandableComposite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.Section;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.CustomSashForm;

public class SettingsPage extends InifileEditorFormPage {
	private FormToolkit formToolkit = null;
	private SashForm sashform = null;
	private Section section1 = null;
	
	public SettingsPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, SWT.V_SCROLL, inifileEditor);
		initialize();
	}

	private void initialize() {
		setFormTitle("Inputs");
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout());
		createSashForm();
		createInputFilesSection();
		
	}

	private void createInputFilesSection() {
		section1 = getFormToolkit().createSection(sashform,
				Section.DESCRIPTION | ExpandableComposite.TITLE_BAR);
		section1.setLayoutData(new GridData(GridData.FILL, GridData.FILL, true, true));
		section1.setText("Configuration Settings");
		section1.setDescription("Edit configuration settings below.");
		section1.setExpanded(true);
		Table x = new Table(section1, SWT.NONE); //XXX just temp
		section1.setClient(x);
	}

	private void createSashForm() {
		sashform = new CustomSashForm(getBody(), SWT.VERTICAL | SWT.SMOOTH);
		getFormToolkit().adapt(sashform);
		//sashform.setBackground(this.getBackground());
		sashform.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL |
											GridData.GRAB_VERTICAL |
											GridData.FILL_BOTH));
	}
	
	private FormToolkit getFormToolkit() {
		if (formToolkit == null)
			formToolkit = new FormToolkit(Display.getCurrent());
		return formToolkit;
	}
}
