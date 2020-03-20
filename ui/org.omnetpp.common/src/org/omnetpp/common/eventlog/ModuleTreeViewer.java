/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.eventlog;

import java.util.Arrays;

import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.IBaseLabelProvider;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.ITreeViewerListener;
import org.eclipse.jface.viewers.TreeExpansionEvent;
import org.eclipse.jface.viewers.TreeViewerColumn;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.TreeColumn;

public class ModuleTreeViewer extends CheckboxTreeViewer {

    private ModuleTreeItem root;

    public ModuleTreeViewer(Composite parent, ModuleTreeItem root) {
        this(parent, SWT.CHECK | SWT.BORDER, root);
    }

    public ModuleTreeViewer(Composite parent, int style, ModuleTreeItem root) {
        super(parent, style);
        this.root = root;
        initialize(parent);
    }

    public void expandChecked() {
        ITreeContentProvider treeContentProvider = (ITreeContentProvider)getContentProvider();
        for (Object element : getCheckedElements()) {
            element = treeContentProvider.getParent(element);
            while (element != null) {
                expandToLevel(element, 1);
                element = treeContentProvider.getParent(element);
            }
        }
    }

    @Override
    public void setLabelProvider(IBaseLabelProvider labelProvider) {
        if (labelProvider instanceof ITableLabelProvider) {
            TreeViewerColumn c1 = new TreeViewerColumn(this, SWT.NONE);
            TreeViewerColumn c2 = new TreeViewerColumn(this, SWT.NONE);
            TreeViewerColumn c3 = new TreeViewerColumn(this, SWT.NONE);
            TreeViewerColumn c4 = new TreeViewerColumn(this, SWT.NONE);
            c1.getColumn().setText("Name");
            c2.getColumn().setText("ID");
            c3.getColumn().setText("Type");
            c4.getColumn().setText("Path");
            getTree().setHeaderVisible(true);
            getTree().setLinesVisible(true);
        }
        super.setLabelProvider(labelProvider);
        packColumns();
    }

    private void initialize(Composite parent) {
        addTreeListener(new ITreeViewerListener() {
            @Override
            public void treeCollapsed(TreeExpansionEvent event) {
                packColumns();
            }

            @Override
            public void treeExpanded(TreeExpansionEvent event) {
                packColumns();
            }
        });
        setContentProvider(new ITreeContentProvider() {
            public void dispose() { }
            public void inputChanged(Viewer viewer, Object oldInput, Object newInput) { }
            public Object[] getChildren(Object parentElement) {
                if (parentElement instanceof String)
                    return new Object[] {root};
                else {
                    ModuleTreeItem[] submods = ((ModuleTreeItem)parentElement).getSubmodules();
                    Arrays.sort(submods, 0, submods.length);
                    return submods;
                }
            }
            public Object getParent(Object element) {
                if (element instanceof String)
                    return null;
                else if (element == root)
                    return "root";
                else
                    return ((ModuleTreeItem)element).getParentModule();
            }
            public boolean hasChildren(Object element) {
                if (element instanceof String)
                    return true;
                else
                    return ((ModuleTreeItem)element).getSubmodules().length > 0;
            }
            public Object[] getElements(Object inputElement) {
                return getChildren(inputElement);
            }
        });

        setLabelProvider(new ILabelProvider() {
            public void addListener(ILabelProviderListener listener) { }
            public void removeListener(ILabelProviderListener listener) { }
            public void dispose() { }
            public boolean isLabelProperty(Object element, String property) {
                return false;
            }
            public Image getImage(Object element) {
                return null;
            }
            public String getText(Object element) {
                ModuleTreeItem mod = (ModuleTreeItem)element;
                String text = mod.getModuleName();
                text += " -";
                if (mod.getNedTypeName() != null)
                    text += " (" + mod.getNedTypeName() + ")";
                text += " " + mod.getModuleFullPath();
                if (mod.getModuleId() != -1)
                    text += " (id = " + mod.getModuleId() + ")";
                return text;
            }

        });

        setInput("root");
        expandAll();

        if (root != null)
            reveal(root); // scroll to top

        Control control = getControl();
        Menu menu = new Menu(control);
        MenuItem menuItem;

        menuItem = new MenuItem(menu, SWT.NONE);
        menuItem.setText("Expand Subtree");
        menuItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                for (Object element : getSelectionFromWidget())
                    expandToLevel(element, ALL_LEVELS);
            }
        });

        menuItem = new MenuItem(menu, SWT.NONE);
        menuItem.setText("Expand Children");
        menuItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                for (Object element : getSelectionFromWidget())
                    for (ModuleTreeItem moduleTreeItem : ((ModuleTreeItem)element).getSubmodules())
                        expandToLevel(moduleTreeItem, ALL_LEVELS);
            }
        });

        menuItem = new MenuItem(menu, SWT.SEPARATOR);

        menuItem = new MenuItem(menu, SWT.NONE);
        menuItem.setText("Collapse Subtree");
        menuItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                for (Object element : getSelectionFromWidget())
                    collapseToLevel(element, ALL_LEVELS);
            }
        });

        menuItem = new MenuItem(menu, SWT.NONE);
        menuItem.setText("Collapse Children");
        menuItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                for (Object element : getSelectionFromWidget())
                    for (ModuleTreeItem moduleTreeItem : ((ModuleTreeItem)element).getSubmodules())
                        collapseToLevel(moduleTreeItem, ALL_LEVELS);
            }
        });

        menuItem = new MenuItem(menu, SWT.SEPARATOR);

        menuItem = new MenuItem(menu, SWT.NONE);
        menuItem.setText("Check Children");
        menuItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                for (Object element : getSelectionFromWidget()) {
                    setChildrenChecked((ModuleTreeItem)element, true);
                    fireCheckStateChanged(element);
                }
            }
        });

        menuItem = new MenuItem(menu, SWT.NONE);
        menuItem.setText("Check Leaves");
        menuItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                for (Object element : getSelectionFromWidget()) {
                    setLeafChecked((ModuleTreeItem)element, true);
                    fireCheckStateChanged(element);
                }
            }
        });

        menuItem = new MenuItem(menu, SWT.NONE);
        menuItem.setText("Check Subtree");
        menuItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                for (Object element : getSelectionFromWidget()) {
                    setSubtreeChecked(element, true);
                    fireCheckStateChanged(element);
                }
            }
        });

        menuItem = new MenuItem(menu, SWT.SEPARATOR);

        menuItem = new MenuItem(menu, SWT.NONE);
        menuItem.setText("Uncheck Children");
        menuItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                for (Object element : getSelectionFromWidget()) {
                    setChildrenChecked((ModuleTreeItem)element, false);
                    fireCheckStateChanged(element);
                }
            }
        });

        menuItem = new MenuItem(menu, SWT.NONE);
        menuItem.setText("Uncheck Leaves");
        menuItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                for (Object element : getSelectionFromWidget()) {
                    setLeafChecked((ModuleTreeItem)element, false);
                    fireCheckStateChanged(element);
                }
            }
        });

        menuItem = new MenuItem(menu, SWT.NONE);
        menuItem.setText("Uncheck Subtree");
        menuItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                for (Object element : getSelectionFromWidget()) {
                    setSubtreeChecked(element, false);
                    fireCheckStateChanged(element);
                }
            }
        });

        control.setMenu(menu);
    }

    private void setLeafChecked(ModuleTreeItem element, boolean checked) {
        ModuleTreeItem[] submodules = element.getSubmodules();

        if (submodules.length == 0)
            setChecked(element, checked);
        else
            for (ModuleTreeItem child : submodules)
                setLeafChecked(child, checked);
    }

    private void setChildrenChecked(ModuleTreeItem element, boolean checked) {
        for (ModuleTreeItem child : element.getSubmodules())
            setChecked(child, checked);
    }

    private void fireCheckStateChanged(Object element) {
        fireCheckStateChanged(new CheckStateChangedEvent(this, element, getChecked(element)));
    }

    private void packColumns() {
        Display.getCurrent().asyncExec(new Runnable() {
            @Override
            public void run() {
                for (TreeColumn column : getTree().getColumns())
                    column.pack();
            }
        });
    }
}
