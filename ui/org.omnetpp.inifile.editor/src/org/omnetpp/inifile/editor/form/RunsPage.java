package org.omnetpp.inifile.editor.form;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.inifile.editor.editors.InifileEditor;

/**
 * Inifile editor page to manage the [Run X] sections in the file.
 * 
 * @author Andras
 */
public class RunsPage extends FormPage {
	private static Font titleFont = new Font(null, "Arial", 10, SWT.BOLD);
	
	public RunsPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, inifileEditor);
		GridLayout gridLayout = new GridLayout(1,false);
		gridLayout.verticalSpacing = 0;
		setLayout(gridLayout);
		
		// title area (XXX a bit ugly -- re-think layout)
		Composite titleArea = new Composite(this, SWT.BORDER);
		GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
		gridData.heightHint = 40;
		titleArea.setLayoutData(gridData);
		titleArea.setBackground(ColorFactory.asColor("white"));
		titleArea.setLayout(new GridLayout(2, false));
		Label imageLabel = new Label(titleArea, SWT.NONE);
		imageLabel.setImage(ImageFactory.getImage(ImageFactory.MODEL_IMAGE_FOLDER)); //XXX 
		imageLabel.setBackground(ColorFactory.asColor("white"));
		Label title = new Label(titleArea, SWT.NONE);
		title.setText("Manage Run Configurations");
		title.setFont(titleFont);
		title.setBackground(ColorFactory.asColor("white"));
		title.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, true));
		new Label(this, SWT.NONE);

		Label l = new Label(this, SWT.NONE);  //XXX
		l.setText("TBD...");
	}
	
	@Override
	public void reread() {
		//TODO
	}

	@Override
	public void commit() {
		//TODO
	}
}
