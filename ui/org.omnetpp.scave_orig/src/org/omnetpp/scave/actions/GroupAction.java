/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.Collection;
import java.util.Iterator;

import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Groups the selected objects. The objects must be siblings and
 * form a contiguous range.
 */
public class GroupAction extends AbstractScaveAction {
    public GroupAction() {
        setText("Group");
        setToolTipText("Surround selected items with a group item");
    }

    @Override
    protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
        RangeSelection range = asRangeSelection(selection);
        if (range != null) {
            Collection<?> elements = selection.toList();
            Group group = ScaveModelFactory.eINSTANCE.createGroup();
            CompoundCommand command = new CompoundCommand("Group");
            command.append(RemoveCommand.create(
                            editor.getEditingDomain(),
                            elements));
            command.append(AddCommand.create(
                            editor.getEditingDomain(),
                            range.owner,
                            range.feature,
                            group,
                            range.fromIndex));
            command.append(AddCommand.create(
                            editor.getEditingDomain(),
                            group,
                            ScaveModelPackage.eINSTANCE.getGroup_Items(),
                            elements));
            editor.executeCommand(command);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        // elements must be contiguous siblings, and must be under a Dataset
        RangeSelection range = asRangeSelection(selection);
        return range != null && ScaveModelUtil.findEnclosingOrSelf(range.owner, Dataset.class) != null;
    }

    static class RangeSelection {
        public EObject owner;
        public EStructuralFeature feature;
        public int fromIndex;
        public int toIndex;
    }

    @SuppressWarnings("unchecked")
    private static RangeSelection asRangeSelection(IStructuredSelection selection) {
        if (selection == null || selection.size() == 0 || !containsEObjectsOnly(selection))
            return null;

        RangeSelection range = new RangeSelection();
        Iterator<? extends EObject> elements = selection.iterator();
        while (elements.hasNext()) {
            EObject element = elements.next();
            EObject owner = element.eContainer();
            EStructuralFeature feature = element.eContainingFeature();
            if (feature == null || !feature.isMany())
                return null;
            int index = ((EList<?>)owner.eGet(feature)).indexOf(element);

            if (range.owner == null) { // first iteration
                range.owner = owner;
                range.feature = feature;
                range.fromIndex = range.toIndex = index;
            }
            else if (range.owner == owner && range.feature == feature) { // sibling
                range.fromIndex = Math.min(range.fromIndex, index);
                range.toIndex = Math.max(range.toIndex, index);
            }
            else // not sibling
                return null;
        }

        if (selection.size() != range.toIndex - range.fromIndex + 1)
            return null;

        return range;
    }
}
