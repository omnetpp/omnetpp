package org.omnetpp.common.properties;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.image.ImageFactory;

/**
 * Dialog for selecting an icon.
 * 
 * @author Andras
 */
public class ImageSelectionDialog extends Dialog {
	private static final int RIGHT_MARGIN = 30;
	
	// widgets
	private ScrolledComposite scrolledComposite; 
	private Composite imageCanvas; 
	private Button okButton;
	
	// result
	private String selection = null;

	
	public ImageSelectionDialog(Shell parentShell, String initialValue) {
		super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
		selection = initialValue;
	}

	@Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
		shell.setText("Select Image");
    }

	@Override
	protected Control createDialogArea(Composite parent) {
		Composite composite = (Composite) super.createDialogArea(parent);
        composite.setLayout(new GridLayout(1,false));

		scrolledComposite = new ScrolledComposite(parent, SWT.H_SCROLL | SWT.V_SCROLL);
		scrolledComposite.getVerticalBar().setIncrement(10); // mouse wheel step
		GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
		gridData.widthHint = 600;
		gridData.heightHint = 400;
		scrolledComposite.setLayoutData(gridData);

		// create inner composite to the icons
		imageCanvas = new Composite(scrolledComposite, SWT.NONE);
		scrolledComposite.setContent(imageCanvas);
        imageCanvas.setLayout(new RowLayout());

		// handle resizes
		scrolledComposite.addControlListener(new ControlAdapter() {
			public void controlResized(ControlEvent e) {
				layoutForm();
			}
		});
        
		for (final String imageName : ImageFactory.getImageNameList()) {
			Button b = new Button(imageCanvas, SWT.FLAT);
			b.setImage(ImageFactory.getImage(imageName));
			b.setToolTipText(imageName);
			b.addSelectionListener(new SelectionListener() {
				public void widgetSelected(SelectionEvent e) {
					imageSelected(imageName);
				}

				public void widgetDefaultSelected(SelectionEvent e) {
					imageSelected(imageName);
					okPressed();
				}
			});
		}
        
        
//        Group group1 = createGroup(composite, "Name and description");
//        
//		// section name field
//		createLabel(group1, "Section Name:", parent.getFont());
//		sectionNameText = new Text(group1, SWT.SINGLE | SWT.BORDER);
//		sectionNameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
//
//		// description field
//		createLabel(group1, "Description (optional):", parent.getFont());
//		descriptionText = new Text(group1, SWT.SINGLE | SWT.BORDER);
//		descriptionText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
//
//        Group group2 = createGroup(composite, "Basic configuration");
//
//		// "extends" section field
//		createLabel(group2, "Fall back to section:", parent.getFont());
//		extendsCombo = new Combo(group2, SWT.READ_ONLY | SWT.BORDER);
//		extendsCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
//
//		// network name field
//		createLabel(group2, "NED Network:", parent.getFont());
//		networkCombo = new Combo(group2, SWT.BORDER);
//		networkCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
//
//		// fill "sections" combo
//		//XXX don't offer sections which would create a circle!!!!!!!!
//		String[] sectionNames = doc.getSectionNames();
//		if (sectionNames.length==0) 
//			sectionNames = new String[] {"General"};  //XXX we lie that [General] exists
//		extendsCombo.setItems(sectionNames);
//		extendsCombo.setVisibleItemCount(Math.min(20, extendsCombo.getItemCount()));
//		extendsCombo.select(0);
//
//		// fill network combo
//		Set<String> networkNames = NEDResourcesPlugin.getNEDResources().getNetworkNames();
//		networkCombo.setItems(networkNames.toArray(new String[] {}));
//		networkCombo.setVisibleItemCount(Math.min(20, networkCombo.getItemCount()));
//		
//		// fill dialog fields with initial contents
//		if (newSectionName!=null) sectionNameText.setText(newSectionName);
//        if (description!=null) descriptionText.setText(description);
//        if (extendsSection!=null) extendsCombo.setText(extendsSection); 
//        if (networkName!=null) networkCombo.setText(networkName);
//		sectionNameText.selectAll();
//		descriptionText.selectAll();
//        
//        // set up validation on content changes
//        ModifyListener listener = new ModifyListener() {
//            public void modifyText(ModifyEvent e) {
//                validateDialogContents();
//            }
//        };
//        sectionNameText.addModifyListener(listener);
//
//        // focus on first field
//		sectionNameText.setFocus();
//
		return composite;
	}

	protected void layoutForm() {
		if (scrolledComposite.getSize().x != 0) {
			// reset size of content so children can be seen (method 1)
			imageCanvas.setSize(imageCanvas.computeSize(scrolledComposite.getSize().x-RIGHT_MARGIN, SWT.DEFAULT));
			imageCanvas.layout();
		}
	}
	
	protected void createButtonsForButtonBar(Composite parent) {
        okButton = createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
    }
	
	protected void imageSelected(String imageName) {
		selection = imageName;
	}

	protected void okPressed() {
        super.okPressed();  // nothing to do here
    }

	public String getFirstResult() {
		return selection;
	}
	
}
