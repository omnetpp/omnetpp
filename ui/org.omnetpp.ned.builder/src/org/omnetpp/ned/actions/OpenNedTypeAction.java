package org.omnetpp.ned.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.ElementListSelectionDialog;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.resources.NEDResources;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

public class OpenNedTypeAction extends Action {
	public OpenNedTypeAction() {
		setText("Open NED Type");
		setToolTipText("Open NED Type");
		setImageDescriptor(NEDResourcesPlugin.getImageDescriptor("icons/full/etool16/opennedtype.gif"));
	}

	public void run() {
		NEDResources ned = NEDResourcesPlugin.getNEDResources();
		
		// pop up a chooser dialog
		IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
		ElementListSelectionDialog dialog = new ElementListSelectionDialog(window.getShell(), new LabelProvider() {
			@Override
			public Image getImage(Object element) {
				INEDTypeInfo nedType = (INEDTypeInfo) element;
				return NEDTreeUtil.getNedModelLabelProvider().getImage(nedType.getNEDElement());
			}

			@Override
			public String getText(Object element) {
				INEDTypeInfo nedType = (INEDTypeInfo) element;
				String typeName = nedType.getNEDElement().getTagName().replace('-', ' ');
				return nedType.getName() + " -- " + typeName;
			}
			
		}); 
		dialog.setElements(ned.getAllComponents().toArray());
		dialog.setMessage("Select NED type to open:");
		dialog.setTitle("Open NED Type");
		if (dialog.open() == ListDialog.OK) {
			INEDTypeInfo component = (INEDTypeInfo) dialog.getResult()[0];
			NEDResourcesPlugin.openNEDElementInEditor(component.getNEDElement());  //XXX null?
		}
	}

}
