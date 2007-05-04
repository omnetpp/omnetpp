package org.omnetpp.common.properties;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;
import java.util.regex.Pattern;
import java.util.regex.PatternSyntaxException;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.image.ImageConverter;
import org.omnetpp.common.image.ImageFactory;

/**
 * Dialog for selecting an icon.
 * 
 * @author Andras
 */
public class ImageSelectionDialog extends Dialog {
	private static final int HEIGHT = 350;
	private static final int WIDTH = 500;
	private static final int RIGHT_MARGIN = 30;

	// widgets
	private Combo filterCombo;
	private Label statusLabel;
	private ScrolledComposite scrolledComposite; 
	private Composite imageCanvas; 
	private Button okButton;
	
	// state
	private String[] imageNames;

	// result
	private String selection = null;


	public ImageSelectionDialog(Shell parentShell, String initialValue) {
		super(parentShell);
		setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
		selection = initialValue;
		imageNames = ImageFactory.getImageNameList().toArray(new String[]{});
	}

	@Override
	protected void configureShell(Shell shell) {
		super.configureShell(shell);
		shell.setText("Select Image");
	}

	@Override
	protected Control createDialogArea(Composite parent) {
		Composite dialogArea = (Composite) super.createDialogArea(parent);

		Composite composite = new Composite(dialogArea, SWT.NONE);
		composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		composite.setLayout(new GridLayout(1,false));

		// label and filter combo
		Label label = new Label(composite, SWT.NONE);
		label.setText("Select Image (substring search, ? = any character, * = any string)");
		label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		label.setFont(parent.getFont());
		filterCombo = new Combo(composite, SWT.BORDER);
		filterCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

		statusLabel = new Label(composite, SWT.NONE);
		label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		label.setFont(parent.getFont());
		
		// scrolled composite for the images
		scrolledComposite = new ScrolledComposite(composite, SWT.BORDER | SWT.H_SCROLL | SWT.V_SCROLL);
		scrolledComposite.getVerticalBar().setIncrement(10); // mouse wheel step
		GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
		gridData.widthHint = WIDTH;
		gridData.heightHint = HEIGHT;
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

		filterCombo.setItems(getImageFilters());
		filterCombo.setVisibleItemCount(Math.min(20, filterCombo.getItemCount()));
		
		// add the images
		populate();

        // set up validation on content changes
        filterCombo.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                populate();
            }
        });
		
		// focus on first field
		filterCombo.setFocus();

		return dialogArea;
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

	protected String[] getImageFilters() {
		ArrayList<String> result = new ArrayList<String>();
		result.add("");
		
		// collect toplevel folders
		Set<String> uniqueFolders = new HashSet<String>();
		for (String name : imageNames)
			if (name.contains("/"))
				uniqueFolders.add(name.replaceFirst("/.*", "/"));
		String[] folders = uniqueFolders.toArray(new String[]{});
		Arrays.sort(folders);
		result.addAll(Arrays.asList(folders));

		// add all concrete images as well
		result.add("--------");
		result.addAll(Arrays.asList(imageNames));

		return result.toArray(new String[]{});
	}

	protected void populate() {
		long startTime = System.currentTimeMillis();

		// regex-ify the filter string
		String filter = filterCombo.getText().trim();
		filter = filter.replace("\\", "\\\\");
		filter = filter.replace(".", "\\.");
		filter = filter.replace("[", "\\[");
		filter = filter.replace("]", "\\]");
		filter = filter.replace("(", "\\(");
		filter = filter.replace(")", "\\)");
		filter = filter.replace("?", ".?");
		filter = filter.replace("*", ".*");
		filter = ".*" + filter + ".*";

		Pattern pattern = null;
		try {
			pattern = Pattern.compile(filter);
		} 
		catch (PatternSyntaxException e) {
			statusLabel.setText("Invalid filter string");
			CommonPlugin.logError(e);
			layoutForm();
			return;
		}
		
		// remove existing images
		for (Control c : imageCanvas.getChildren())
			c.dispose();

		// add new images
		int count = 0;
		String theImage = null;
		for (final String imageName : imageNames) {
			if (pattern.matcher(imageName).matches()) {
				count++;
				theImage = imageName;

				// produce image and tooltip
				Image image = ImageFactory.getImage(imageName);
				Rectangle bounds = image.getBounds();
				String tooltip = imageName + " (" + bounds.width + "x" + bounds.height + ")";
				if (bounds.width > 64 || bounds.height > 64) {
					image = ImageConverter.getResampledImage(image, 64, 64);  //XXX will have to be disposed!!!!
					tooltip += " [scaled back for display]";
				}

				// create button
				Button button = new Button(imageCanvas, SWT.PUSH);
				button.setImage(image);
				button.setToolTipText(tooltip);
				button.addSelectionListener(new SelectionListener() {
					public void widgetSelected(SelectionEvent e) {
						imageSelected(imageName);
					}

					public void widgetDefaultSelected(SelectionEvent e) {
						//XXX never called for a Button -- sorry...
						imageSelected(imageName);
						okPressed();
					}
				});
			}
		}
		
		layoutForm();
		
		// update status line
		statusLabel.setText("Filter matches "+count+" images out of "+imageNames.length+".");
		
		// if selection got narrowed down to one image: make that the user's choice;
		// otherwise invalidate user's previous selection
		imageSelected(count==1 ? theImage : null);
		
		System.out.println("Dialog refresh: "+(System.currentTimeMillis()-startTime)+"ms");
	}

	protected void imageSelected(String imageName) {
		selection = imageName;
		if (okButton != null)  // it is null initially
			okButton.setEnabled(selection != null);
	}

	protected void okPressed() {
		super.okPressed();  // nothing to do here
	}

	public String getFirstResult() {
		return selection;
	}
}
