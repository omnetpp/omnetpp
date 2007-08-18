package org.omnetpp.common.properties;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.ElementListSelectionDialog;
import org.omnetpp.common.image.ImageFactory;

public class ImageSelectionDialog2 extends ElementListSelectionDialog {

	protected String initialSelection = null;

	/**
	 * Text and icon provider
	 */
	public static class ImageFactoryLabelProvider extends LabelProvider {

		@Override
		public String getText(Object element) {
			if ("".equals(element))
				return "(none/default)";
			return super.getText(element);
		}

		@Override
		public Image getImage(Object element) {
			if ("".equals(element))
				return null;

			return ImageFactory.getImage((String)element, "", null, 0) ;
		}
	}

	public ImageSelectionDialog2(Shell parentShell, String initialValue) {
		super(parentShell, new ImageFactoryLabelProvider());

		initialSelection = initialValue;

		setTitle("Select image");
		setAllowDuplicates(false);
		setDialogHelpAvailable(false);
		setMultipleSelection(false);
		setFilter("*");
		setIgnoreCase(true);
		setEmptyListMessage("No image is available");
		// set the initial value
		ArrayList<String> selectedElements = new ArrayList<String>();
		if (initialSelection != null && !"".equals(initialSelection))
		{
			selectedElements.add(initialSelection);
			setInitialElementSelections(selectedElements);
		}

		// set the list to be displayed
		List<String> elements = ImageFactory.getImageNameList();
		// add an empty image to be able to clear
		elements.add(0, "");
		setElements(elements.toArray());
	}


	@Override
	protected Label createMessageArea(Composite composite) {
		// we do not need the message part...
        return null;
	}

}
