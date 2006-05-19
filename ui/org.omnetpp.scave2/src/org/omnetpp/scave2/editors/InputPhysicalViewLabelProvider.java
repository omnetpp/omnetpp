package org.omnetpp.scave2.editors;

import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.model.Inputs;

public class InputPhysicalViewLabelProvider implements ILabelProvider {

	public Image getImage(Object element) {
		return null;
	}

	public String getText(Object element) {
		if (element instanceof Inputs)
			return "Inputs";
		else if (element instanceof File) {
			File file = (File)element;
			return file.getFilePath();
		}
		else if (element instanceof Run) {
			Run run = (Run)element;
			return run.getRunName() + ", " + run.getDate();
		}
		return "";
	}

	public void dispose() {
	}

	public boolean isLabelProperty(Object element, String property) {
		return false;
	}

	public void addListener(ILabelProviderListener listener) {
	}

	public void removeListener(ILabelProviderListener listener) {
	}
}
