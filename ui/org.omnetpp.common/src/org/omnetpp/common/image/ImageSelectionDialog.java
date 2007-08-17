package org.omnetpp.common.image;

import java.util.ArrayList;
import java.util.List;
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
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.CommonPlugin;

/**
 * Dialog for selecting an icon. To ensure responsiveness, dialog get filled 
 * asynchronously if it would take too long.
 * 
 * @author Andras
 */
public class ImageSelectionDialog extends Dialog {
	private static final int HEIGHT = 350;
	private static final int WIDTH = 500;
	private static final int RIGHT_MARGIN = 30;
	
	private static final long TIMEOUT_MILLIS = 500;

	// widgets
	private Combo filterCombo;
	private Label filterStatusLabel;
	private Label selectionStatusLabel;
	private ScrolledComposite scrolledComposite; 
	private Composite imageCanvas; 
	private Button okButton;

	// state
	private List<String> imageNames;
	private int currentPopulationNumber = 0;

	// result
	private String initialSelection = null;
	private String selection = null;


	public ImageSelectionDialog(Shell parentShell, String initialValue) {
		super(parentShell);
		setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
		initialSelection = "".equals(initialValue) ? null : initialValue; // store "" as null
		imageNames = new ArrayList<String>(ImageFactory.getImageNameList());
		// "" represents NO IMAGE
		imageNames.add(0, "");
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

		filterStatusLabel = new Label(composite, SWT.NONE);
		filterStatusLabel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		filterStatusLabel.setFont(parent.getFont());

		selectionStatusLabel = new Label(composite, SWT.NONE);
		selectionStatusLabel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		selectionStatusLabel.setFont(parent.getFont());

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
		if (initialSelection != null)
			filterCombo.setText(initialSelection.replaceFirst("/.*", "/"));

		// set up validation on content changes
		filterCombo.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				populate(null);
			}
		});

		// focus on first field
		filterCombo.setFocus();

		// add the images, and make initial icon one selected
		populate(initialSelection);

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

		// collect toplevel folders
		result.addAll(ImageFactory.getCategories());

		// add all concrete images as well
		result.addAll(imageNames);

		return result.toArray(new String[]{});
	}

	protected void populate(String imageToSelect) {
		// regex'ify the filter string
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
			filterStatusLabel.setText("Invalid filter string");
			CommonPlugin.logError(e);
			layoutForm();
			return;
		}

		// remove existing images
		for (Control c : imageCanvas.getChildren())
			c.dispose();

		// add new images
		int count = 0;
		String oneImage = null;
		long startTime = System.currentTimeMillis();
		currentPopulationNumber++; 
		final int schedulePopulationNumber = currentPopulationNumber;
		for (final String imageName : imageNames) {
			if (pattern.matcher(imageName).matches() || imageName.equals("")) {
  			    if (!imageName.equals("")) {
  			    	count++;
  			    	oneImage = imageName; 
  			    }
  			    
  			    // add image to canvas: either directly, or if we're taking too much time already,
  			    // defer it using Display.asyncExec(). NOTE: we use "population number" to ensure
  			    // that a populate() call cancels stale asyncExec()'s still pending from the
  			    // previous populate()
  			    if (System.currentTimeMillis() - startTime <= TIMEOUT_MILLIS) {
  			    	addImageToCanvas(imageName);
  			    }
  			    else {
  			    	Display.getCurrent().asyncExec(new Runnable() {
  			    		public void run() {
  			    			if (!imageCanvas.isDisposed() && currentPopulationNumber==schedulePopulationNumber) {
  			    				addImageToCanvas(imageName);
  			    				layoutForm();
  			    			}
  			    		}
  			    	});
  			    }
			}
		}

		layoutForm();

		// update status line
		filterStatusLabel.setText("Filter matches "+count+" image"+(count==1?"":"s")+" out of "+imageNames.size()+".");

		// if selection got narrowed down to one image: make that the user's choice;
		// otherwise invalidate user's previous selection
		final String finalImageToSelect = (count==1) ? oneImage : imageToSelect;

		if (finalImageToSelect != null) {
			Display.getCurrent().asyncExec(new Runnable() {
				public void run() {
					if (!imageCanvas.isDisposed() && currentPopulationNumber==schedulePopulationNumber) {
						selectImage(finalImageToSelect);
					}
				}
			});
		}
	}

	protected void addImageToCanvas(final String imageName) {
		Button button = new Button(imageCanvas, SWT.PUSH);
		
		if (imageName.equals("")) {
		    button.setText("NONE");
		    button.setLayoutData(new RowData(50,50));
		    button.setToolTipText("Clear existing image");
		} 
		else {
		    // produce image and tooltip
		    Image image = ImageFactory.getImage(imageName);
		    Rectangle bounds = image.getBounds();
		    String tooltip = imageName + " (" + bounds.width + "x" + bounds.height + ")";
		    if (bounds.width > 64 || bounds.height > 64) {
		        image = ImageConverter.getResampledImage(image, 64, 64);  //FIXME will have to be disposed!!!! cache it!!!
		        tooltip += " [scaled back for display]";
		    }

		    button.setImage(image);
		    button.setToolTipText(tooltip);
		}
		
		button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				selectImage(imageName);
			}
		});
		// unfortunately, widgetDefaultSelected() does not work for a button, so:
		button.addMouseListener(new MouseAdapter() {
			public void mouseDoubleClick(MouseEvent e) {
				selectImage(imageName);
				okPressed();
			}
		});
	}

	protected void selectImage(String imageName) {
		selection = imageName;
		selectionStatusLabel.setText("Current selection: "+(selection==null ? "--" : selection));
		if (okButton != null)  // it is null initially
			okButton.setEnabled(selection != null);
	}

	public String getImageId() {
		return selection;
	}
}
