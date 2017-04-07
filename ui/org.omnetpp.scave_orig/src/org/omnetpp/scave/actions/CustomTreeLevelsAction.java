package org.omnetpp.scave.actions;

import java.util.Arrays;
import java.util.Comparator;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.scave.editors.datatable.DataTree;
import org.omnetpp.scave.editors.datatable.ResultFileManagerTreeContentProvider;

@SuppressWarnings("unchecked")
public class CustomTreeLevelsAction extends Action {
    private DataTree dataTree;

    public CustomTreeLevelsAction(DataTree dataTree) {
        super("Custom...", Action.AS_RADIO_BUTTON);
        this.dataTree = dataTree;
    }

    @Override
    public boolean isChecked() {
        ResultFileManagerTreeContentProvider contentProvider = dataTree.getContentProvider();
        Class[] levels = contentProvider.getLevels();
        boolean checked = true;
        for (Class[] predefinedLevels : new Class[][] { ResultFileManagerTreeContentProvider.LEVELS1, ResultFileManagerTreeContentProvider.LEVELS2, ResultFileManagerTreeContentProvider.LEVELS3, ResultFileManagerTreeContentProvider.LEVELS4, ResultFileManagerTreeContentProvider.LEVELS5, ResultFileManagerTreeContentProvider.LEVELS6 })
            if (PredefinedLevelsAction.isLevelsEqualsIgnoreModuleNameLevel(levels, predefinedLevels))
                checked = false;
        setChecked(checked);
        return super.isChecked();
    }

    @Override
    public void run() {
        TitleAreaDialog dialog = new TitleAreaDialog(Display.getCurrent().getActiveShell()) {
            protected CheckboxTableViewer viewer;

            // constructor
            {
                setShellStyle(getShellStyle() | SWT.RESIZE);
            }

            @Override
            protected void configureShell(Shell shell) {
                super.configureShell(shell);
                shell.setText("Configure Tree Levels");
            }

            @Override
            protected Control createDialogArea(Composite parent) {
                Composite container = (Composite)super.createDialogArea(parent);
                setTitle("Select levels");
                setMessage("Select and order tree levels");
                Composite content = new Composite(container, SWT.BORDER);
                content.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1));
                content.setLayout(new GridLayout(2, false));
                Label header = new Label(content, SWT.NONE);
                header.setText("Available tree levels:");
                header.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 2, 1));
                viewer = CheckboxTableViewer.newCheckList(content, SWT.BORDER);
                viewer.setContentProvider(new ArrayContentProvider());
                viewer.setLabelProvider(new LabelProvider() {
                    @Override
                    public String getText(Object element) {
                        try {
                            return (String)((Class)element).getMethod("getLevelName").invoke(null);
                        }
                        catch (Exception e) {
                            throw new RuntimeException(e);
                        }
                    }
                });
                final Class[] levels = dataTree.getContentProvider().getLevels();
                Class[] levelClasses = ResultFileManagerTreeContentProvider.getAvailableLevelClasses();
                Arrays.sort(levelClasses, new Comparator<Class>() {
                    public int compare(Class l1, Class l2) {
                        return adjust(ArrayUtils.indexOf(levels, l1)) - adjust(ArrayUtils.indexOf(levels, l2));
                    }

                    private int adjust(int pos) {
                        return pos == -1 ? Integer.MAX_VALUE : pos;
                    }
                });
                viewer.setInput(levelClasses);
                viewer.setCheckedElements(levels);
                viewer.getControl().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1));
                Composite buttons = new Composite(content, SWT.NONE);
                GridLayout layout = new GridLayout(1, false);
                layout.marginWidth = layout.marginHeight = 0;
                buttons.setLayout(layout);
                buttons.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
                createButton(buttons, IDialogConstants.CLIENT_ID, "Up", true);
                createButton(buttons, IDialogConstants.CLIENT_ID + 1, "Down", true);
                layout.numColumns = 1;
                return container;
            }

            @Override
            protected void createButtonsForButtonBar(Composite parent) {
                super.createButtonsForButtonBar(parent);
            }

            @Override
            protected void buttonPressed(int buttonId) {
                if (buttonId == IDialogConstants.CLIENT_ID) {
                    Object element = ((IStructuredSelection)viewer.getSelection()).getFirstElement();
                    if (element != null) {
                        Object[] elements = (Object[])viewer.getInput();
                        int index = ArrayUtils.indexOf(elements, element);
                        if (index > 0) {
                            Object temp = elements[index - 1];
                            elements[index - 1] = element;
                            elements[index] = temp;
                            viewer.refresh();
                        }
                    }
                }
                else if (buttonId == IDialogConstants.CLIENT_ID + 1) {
                    Object element = ((IStructuredSelection)viewer.getSelection()).getFirstElement();
                    if (element != null) {
                        Object[] elements = (Object[])viewer.getInput();
                        int index = ArrayUtils.indexOf(elements, element);
                        if (index < elements.length - 1) {
                            Object temp = elements[index + 1];
                            elements[index + 1] = element;
                            elements[index] = temp;
                            viewer.refresh();
                        }
                    }
                }
                super.buttonPressed(buttonId);
            }

            @Override
            protected void okPressed() {
                dataTree.setLevels(Arrays.asList(viewer.getCheckedElements()).toArray(new Class[0]));
                super.okPressed();
            }
        };
        dialog.open();
    }
}