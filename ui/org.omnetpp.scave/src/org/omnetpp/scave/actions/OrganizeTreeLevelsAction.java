package org.omnetpp.scave.actions;

import java.util.Arrays;
import java.util.Comparator;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.scave.editors.datatable.DataTree;
import org.omnetpp.scave.editors.datatable.ResultFileManagerTreeContentProvider;

public class OrganizeTreeLevelsAction extends Action {
    private DataTree dataTree;

    public OrganizeTreeLevelsAction(DataTree dataTree) {
        this.dataTree = dataTree;
    }

    @Override
    public String getText() {
        return "Organize Tree Levels";
    }

    @SuppressWarnings("unchecked")
    @Override
    public void run() {
        TitleAreaDialog dialog = new TitleAreaDialog(Display.getCurrent().getActiveShell()) {
            protected CheckboxTableViewer viewer;

            @Override
            protected Control createDialogArea(Composite parent) {
                Composite container = (Composite)super.createDialogArea(parent);
                setTitle("Select levels");
                setMessage("Select tree levels to organize results");
                viewer = CheckboxTableViewer.newCheckList(container, SWT.BORDER);
                viewer.setContentProvider(new ArrayContentProvider());
                viewer.setLabelProvider(new LabelProvider() {
                    @Override
                    public String getText(Object element) {
                        String className = ((Class)element).getName();
                        String innerClassName = className.substring(className.indexOf("$") + 1);
                        String label = StringUtils.join(org.omnetpp.common.util.StringUtils.splitCamelCaseString(StringUtils.removeEnd(innerClassName, "Node"), ' '), " ");
                        return StringUtils.capitalize(label.toLowerCase());
                    }
                });
                final Class[] levels = dataTree.getContentProvider().getLevels();
                Class[] levelClasses = ResultFileManagerTreeContentProvider.getAvailableLevelClasses();
                Arrays.sort(levelClasses, new Comparator<Class>() {
                    public int compare(Class l1, Class l2) {
                        return ArrayUtils.indexOf(levels, l1) - ArrayUtils.indexOf(levels, l2);
                    }
                });
                viewer.setInput(levelClasses);
                viewer.setCheckedElements(levels);
                viewer.getControl().setLayoutData(new GridData(GridData.FILL_BOTH));
                return container;
            }

            @Override
            protected void createButtonsForButtonBar(Composite parent) {
                createButton(parent, IDialogConstants.CLIENT_ID, "Up", true);
                createButton(parent, IDialogConstants.CLIENT_ID + 1, "Down", true);
                super.createButtonsForButtonBar(parent);
            }

            @Override
            protected void buttonPressed(int buttonId) {
                if (buttonId == IDialogConstants.CLIENT_ID) {
                    Object element = ((IStructuredSelection)viewer.getSelection()).getFirstElement();
                    Object[] elements = (Object[])viewer.getInput();
                    int index = ArrayUtils.indexOf(elements, element);
                    if (index > 0) {
                        Object temp = elements[index - 1];
                        elements[index - 1] = element;
                        elements[index] = temp;
                        viewer.refresh();
                    }
                }
                else if (buttonId == IDialogConstants.CLIENT_ID + 1) {
                    Object element = ((IStructuredSelection)viewer.getSelection()).getFirstElement();
                    Object[] elements = (Object[])viewer.getInput();
                    int index = ArrayUtils.indexOf(elements, element);
                    if (index < elements.length - 1) {
                        Object temp = elements[index + 1];
                        elements[index + 1] = element;
                        elements[index] = temp;
                        viewer.refresh();
                    }
                }
                super.buttonPressed(buttonId);
            }

            @Override
            protected void okPressed() {
                dataTree.getContentProvider().setLevels(Arrays.asList(viewer.getCheckedElements()).toArray(new Class[0]));
                dataTree.refresh();
                super.okPressed();
            }
        };
        dialog.setTitle("Select levels");
        dialog.open();
    }
}