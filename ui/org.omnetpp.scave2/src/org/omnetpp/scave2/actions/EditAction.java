package org.omnetpp.scave2.actions;

import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.ui.EditDialog;

/**
 * Opens an edit dialog for the selected dataset, chart, chart sheet, etc.
 */
public class EditAction extends AbstractScaveAction {
	public EditAction() {
        setText("Edit...");
        setToolTipText("Edit selected item");
	}

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		if (isApplicable(scaveEditor, selection)) {
		
			EObject object = (EObject)selection.getFirstElement(); //TODO edit several objects together?

			EditDialog dialog = new EditDialog(scaveEditor.getSite().getShell(), object);
			if (dialog.open() == Window.OK) {
				CompoundCommand command = new CompoundCommand("Edit");
				EStructuralFeature[] features = dialog.getFeatures();
				for (int i = 0; i < features.length; ++i) {
					Object oldValue = object.eGet(features[i]);
					Object newValue = dialog.getValue(i);
					boolean isDirty = oldValue == null && newValue != null ||
									  oldValue != null && !oldValue.equals(newValue);
					if (isDirty) {
						command.append(SetCommand.create(
							scaveEditor.getEditingDomain(),
							object,
							features[i],
							newValue));
					}
				}
				scaveEditor.executeCommand(command);
			}
		}
	}

	@Override
	public boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return selection.size() == 1 && selection.getFirstElement() instanceof EObject;
	}
}
