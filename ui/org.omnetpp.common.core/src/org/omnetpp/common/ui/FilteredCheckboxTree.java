/*******************************************************************************
 * Copyright (c) 2008, 2010 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Ian Bull <irbull@cs.uvic.ca> - bug 207064
 *******************************************************************************/
//originally: package org.eclipse.pde.internal.ui.launcher;
package org.omnetpp.common.ui;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.ui.dialogs.FilteredTree;
import org.eclipse.ui.dialogs.PatternFilter;
import org.eclipse.ui.progress.WorkbenchJob;

/**
 * A FilteredChecboxTree.  This tree stores all the tree elements internally, and keeps the
 * check state in sync.  This way, even if an element is filtered, the caller can get and set the
 * checked state.
 *
 * The internal representation is additive.  That is, elements are never removed from the internal
 * representation.  This is OK since the PDE launch Dialog never changes the elements once
 * the view is opened.  If any other tree is based on this code, they may want to address this issue.
 *
 * This is not public because it was customized for the Launch Dialog.
 *
 * Code copied from: org.eclipse.pde.internal.ui.launcher.FilteredCheckboxTree. --Andras
 */
@SuppressWarnings({"rawtypes", "unchecked"})
public class FilteredCheckboxTree extends FilteredTree {

    private WorkbenchJob refreshJob;

    /**
     * The FilteredCheckboxTree Constructor.
     * @param parent The parent composite where this Tree will be placed.
     * @param treeStyle Tree styles
     * @param filter The pattern filter that will be used to filter elements
     */
    public FilteredCheckboxTree(Composite parent, int treeStyle, PatternFilter filter) {
        super(parent, treeStyle, filter, true);
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.ui.dialogs.FilteredTree#doCreateTreeViewer(org.eclipse.swt.widgets.Composite, int)
     */
    protected TreeViewer doCreateTreeViewer(Composite parent, int style) {
        return new FilterableCheckboxTreeViewer(parent, style);
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.ui.dialogs.FilteredTree#doCreateRefreshJob()
     */
    protected WorkbenchJob doCreateRefreshJob() {
        // Since refresh job is private, we have to get a handle to it
        // when it is created, and store it locally.
        //
        // See: 218903: [Viewers] support extensibility of the refresh job in FilteredTree
        // https://bugs.eclipse.org/bugs/show_bug.cgi?id=218903
        WorkbenchJob job = super.doCreateRefreshJob();
        refreshJob = job;
        return job;
    }

    /**
     * Resets the filter and returns when the refresh is complete
     */
    public void resetFilter() {
        // Set the next to the initial Text, stop any outstanding jobs
        // and call the refresh job to run synchronously.
        Text filterText = getFilterControl();
        if (filterText != null) {
            filterText.setText(this.initialText);
        }
        refreshJob.cancel();
        refreshJob.runInUIThread(new NullProgressMonitor());
    }

    /**
     * Get the number of pixels the tree viewer is from the top of the filtered
     * checkbox tree viewer.  This is  useful if you wish to align buttons with the
     * tree.
     * @return the offset of the Tree from the top of the container
     */
    int getTreeLocationOffset() {
        GridLayout layout = (GridLayout) getLayout();
        int space = layout.horizontalSpacing + layout.marginTop + ((GridData) getLayoutData()).verticalIndent + 1;
        Text filterText = getFilterControl();
        if (filterText != null) {
            space += filterText.getSize().y;
        }
        return space;
    }

    /**
     * Classes which implement this interface deal with notifications from the
     * filtered checkbox tree viewer.  The notifications are fired before a refresh
     * happens.
     */
    interface PreRefreshNotifier {
        public void preRefresh(FilterableCheckboxTreeViewer viewer, boolean filtered);
    }

    /**
     * A CheckboxTreeViewer that maintains an internal representation of all the nodes.
     *
     */
    public class FilterableCheckboxTreeViewer extends CheckboxTreeViewer {
        static final String NONE = "none"; //$NON-NLS-1$
        static final String CHECKED = "checked"; //$NON-NLS-1$
        static final String GREYED = "greyed"; //$NON-NLS-1$
        static final String CHECKED_GREYED = "checked_greyed"; //$NON-NLS-1$

        /**
         * The internal node for the FilterableCheckboxTreeViewer
         */
        class FilteredCheckboxTreeItem {
            Object data; // Data element
            String state; // Checked State
            List children = new ArrayList();

            public FilteredCheckboxTreeItem(Object data, String state, Map itemCache, FilteredCheckboxTreeItem parent) {
                this.data = data;
                this.state = state;
                itemCache.put(data, this);
                if (parent != null) {
                    parent.children.add(this);
                }
            }
        }

        /* A cache of all the nodes */
        Map itemCache = new HashMap();
        /* The preRefresh Listeners */
        List refreshingListeners = new ArrayList();

        protected void unmapAllElements() {
            itemCache = new HashMap();
            super.unmapAllElements();
        }

        /**
         * FilterableCheckboxTreeViewer constructor. This creates the tree part of the filtered tree.
         */
        public FilterableCheckboxTreeViewer(Composite parent, int style) {
            super(parent, style);
            addCheckStateListener(new ICheckStateListener() {

                public void checkStateChanged(CheckStateChangedEvent event) {
                    FilteredCheckboxTreeItem item = (FilteredCheckboxTreeItem) itemCache.get(event.getElement());
                    if (item != null) {
                        item.state = event.getChecked() ? CHECKED : NONE;
                    }
                }
            });
        }

        /**
         * Allows clients to listen to the tree refresh.
         */
        public void addPreRefreshNotifier(PreRefreshNotifier notifier) {
            if (refreshingListeners.contains(notifier))
                return;
            refreshingListeners.add(notifier);
        }

        /*
         * (non-Javadoc)
         * @see org.eclipse.jface.viewers.CheckboxTreeViewer#getChecked(java.lang.Object)
         */
        public boolean getChecked(Object element) {
            Widget testFindItem = getViewer().testFindItem(element);
            testFindItem = null;
            if (testFindItem == null) {
                if (itemCache.containsKey(element)) {
                    FilteredCheckboxTreeItem item = (FilteredCheckboxTreeItem) itemCache.get(element);
                    if (item.state.equals(CHECKED))
                        return true;
                    if (item.state.equals(CHECKED_GREYED))
                        return true;
                    if (item.state.equals(GREYED))
                        return true;
                    else if (item.state.equals(NONE))
                        return false;
                }
            }
            return super.getChecked(element);
        }

        public Object[] getCheckedChildren(Object element) {
            FilteredCheckboxTreeItem item = (FilteredCheckboxTreeItem) itemCache.get(element);
            List checkedChildren = new ArrayList();
            if (item != null) {
                List children = item.children;
                Iterator iterator = children.iterator();
                while (iterator.hasNext()) {
                    FilteredCheckboxTreeItem child = (FilteredCheckboxTreeItem) iterator.next();
                    if (child.state == CHECKED) {
                        checkedChildren.add(child.data);
                    }
                }
            }
            return checkedChildren.toArray();
        }

        /**
         * DO NOT USE THIS METHOD! It appears to be bogus: simply returns empty list if tree hasn't been filtered
         * --Andras
         */
        public Object[] getCheckedElements() {
            Iterator iterator = itemCache.values().iterator();
            List checkedElements = new LinkedList();
            while (iterator.hasNext()) {
                FilteredCheckboxTreeItem item = (FilteredCheckboxTreeItem) iterator.next();
                Widget testFindItem = getViewer().testFindItem(item.data);
                if (testFindItem == null) {
                    if (item.state.equals(CHECKED) || item.state.equals(CHECKED_GREYED) || item.state.equals(GREYED)) {
                        checkedElements.add(item.data);
                    }
                } else {
                    if (((TreeItem) testFindItem).getChecked()) {
                        checkedElements.add(testFindItem.getData());
                    }
                }
            }
            return checkedElements.toArray();
        }

        /*
         * (non-Javadoc)
         * @see org.eclipse.jface.viewers.CheckboxTreeViewer#setChecked(java.lang.Object, boolean)
         */
        public boolean setChecked(Object element, boolean state) {
            if (itemCache.containsKey(element)) {
                FilteredCheckboxTreeItem item = (FilteredCheckboxTreeItem) itemCache.get(element);
                item.state = state ? CHECKED : NONE;
            }
            return super.setChecked(element, state);
        }

        /*
         * (non-Javadoc)
         * @see org.eclipse.jface.viewers.CheckboxTreeViewer#setCheckedElements(java.lang.Object[])
         */
        public void setCheckedElements(Object[] elements) {
            Set s = new HashSet(itemCache.keySet());
            s.removeAll(new HashSet(Arrays.asList(elements)));
            for (int i = 0; i < elements.length; i++) {
                FilteredCheckboxTreeItem item = (FilteredCheckboxTreeItem) itemCache.get(elements[i]);
                if (item != null) {
                    item.state = CHECKED;
                }
            }
            for (Iterator iterator = s.iterator(); iterator.hasNext();) {
                Object object = iterator.next();
                FilteredCheckboxTreeItem item = (FilteredCheckboxTreeItem) itemCache.get(object);
                if (item != null) {
                    item.state = NONE;
                }
            }
            super.setCheckedElements(elements);
        }

        /*
         * (non-Javadoc)
         * @see org.eclipse.jface.viewers.CheckboxTreeViewer#setSubtreeChecked(java.lang.Object, boolean)
         */
        public boolean setSubtreeChecked(Object element, boolean state) {
            String newState = state ? CHECKED : NONE;
            TreeItem item = (TreeItem) testFindItem(element);
            FilteredCheckboxTreeItem filteredCheckboxTreeItem = (FilteredCheckboxTreeItem) itemCache.get(element);
            if (item != null && filteredCheckboxTreeItem != null) {
                filteredCheckboxTreeItem.state = newState;
                TreeItem[] items = item.getItems();
                for (int i = 0; i < items.length; i++) {
                    item = items[i];
                    if (item != null) {
                        filteredCheckboxTreeItem = (FilteredCheckboxTreeItem) itemCache.get(item.getData());
                        if (filteredCheckboxTreeItem != null) {
                            filteredCheckboxTreeItem.state = newState;
                        }
                    }
                }
            }
            return super.setSubtreeChecked(element, state);
        }

        /*
        public boolean setSubtreeChecked(Object element, boolean state) {
            String newState = state ? CHECKED : NONE;
            FilteredCheckboxTreeItem filteredCheckboxTreeItem = (FilteredCheckboxTreeItem) itemCache.get(element);
            if (filteredCheckboxTreeItem != null) {
                filteredCheckboxTreeItem.state = newState;
                List children = filteredCheckboxTreeItem.children;
                for (Iterator iterator = children.iterator(); iterator.hasNext();) {
                    FilteredCheckboxTreeItem child = (FilteredCheckboxTreeItem) iterator.next();
                    child.state = newState;
                }
            }
            return super.setSubtreeChecked(element, state);
        }
        */

        /*
         * (non-Javadoc)
         * @see org.eclipse.jface.viewers.CheckboxTreeViewer#preservingSelection(java.lang.Runnable)
         */
        protected void preservingSelection(Runnable updateCode) {
            super.preservingSelection(updateCode);

            // Re-apply the checked state
            ArrayList allTreeItems = getAllTreeItems(treeViewer.getTree().getItems());
            for (Iterator iterator = allTreeItems.iterator(); iterator.hasNext();) {
                TreeItem item = (TreeItem) iterator.next();
                doApplyCheckedState(item, item.getData());
            }
        }

        /*
         * (non-Javadoc)
         * @see org.eclipse.jface.viewers.AbstractTreeViewer#internalRefresh(java.lang.Object, boolean)
         */
        protected void internalRefresh(Object element, boolean updateLabels) {
            String text = FilteredCheckboxTree.this.getFilterString();
            boolean initial = initialText != null && initialText.equals(text);
            boolean filtered = (text.length() > 0 && !initial);

            // Notify anybody who is listening for the refresh
            for (Iterator iterator = refreshingListeners.iterator(); iterator.hasNext();) {
                PreRefreshNotifier notifier = (PreRefreshNotifier) iterator.next();
                notifier.preRefresh(FilterableCheckboxTreeViewer.this, filtered);
            }
            saveCheckedState();
            super.internalRefresh(element, updateLabels);
            treeViewer.expandAll();
        }

        /*
         * Set the checked state
         */
        private void doApplyCheckedState(Item item, Object element) {
            // update the item first
            super.doUpdateItem(item, element);

            // Update the checked state
            TreeItem treeItem = (TreeItem) item;
            if (itemCache.containsKey(element)) {
                String state = ((FilteredCheckboxTreeItem) itemCache.get(element)).state;
                if (state.equals(CHECKED_GREYED)) {
                    treeItem.setGrayed(true);
                    treeItem.setChecked(true);
                } else if (state.equals(CHECKED)) {
                    treeItem.setChecked(true);
                    treeItem.setGrayed(false);
                } else if (state.equals(GREYED)) {
                    treeItem.setGrayed(true);
                    treeItem.setChecked(false);
                } else {
                    treeItem.setGrayed(false);
                    treeItem.setChecked(false);
                }
            }
        }

        /*
         * A helper method to get all the items in the tree
         */
        private ArrayList getAllTreeItems(TreeItem[] roots) {
            ArrayList list = new ArrayList();
            for (int i = 0; i < roots.length; i++) {
                TreeItem item = roots[i];
                list.add(item);
                list.addAll(getAllTreeItems(item.getItems()));
            }
            return list;
        }

        /**
         * Saves the checked state of all the elements in the tree
         */
        private void saveCheckedState() {
            TreeItem[] items = treeViewer.getTree().getItems();
            for (int i = 0; i < items.length; i++) {
                TreeItem item = items[i];
                if (!itemCache.containsKey(item.getData())) {
                    new FilteredCheckboxTreeItem(item.getData(), getItemState(item), itemCache, null);
                }
                FilteredCheckboxTreeItem filteredCheckboxTreeItem = (FilteredCheckboxTreeItem) itemCache.get(item.getData());
                filteredCheckboxTreeItem.state = getItemState(item);
                saveCheckedState(filteredCheckboxTreeItem, item);
            }
        }

        /**
         * Saves the checked state of an item and all its children
         */
        private void saveCheckedState(FilteredCheckboxTreeItem parent, TreeItem parentItem) {
            TreeItem[] items = parentItem.getItems();
            for (int i = 0; i < items.length; i++) {
                TreeItem item = items[i];
                if (!itemCache.containsKey(item.getData())) {
                    new FilteredCheckboxTreeItem(item.getData(), getItemState(item), itemCache, parent);
                }
                FilteredCheckboxTreeItem filteredCheckboxTreeItem = (FilteredCheckboxTreeItem) itemCache.get(item.getData());
                filteredCheckboxTreeItem.state = getItemState(item);
                saveCheckedState(filteredCheckboxTreeItem, item);
            }
        }

        /**
         * Computes the checked state from a tree item
         */
        private String getItemState(TreeItem item) {
            if (item.getChecked() && item.getGrayed()) {
                return CHECKED_GREYED;
            } else if (item.getChecked()) {
                return CHECKED;
            } else if (item.getGrayed()) {
                return GREYED;
            } else {
                return NONE;
            }
        }

    } // end of FilterableCheckboxTreeViewer

    public void setEnabled(boolean enabled) {
        if ((filterText.getStyle() & SWT.ICON_CANCEL) == 0) { // filter uses FilteredTree new look, not native
            int filterColor = enabled ? SWT.COLOR_LIST_BACKGROUND : SWT.COLOR_WIDGET_BACKGROUND;
            filterComposite.setBackground(getDisplay().getSystemColor(filterColor));
        }
        filterText.setEnabled(enabled);
        treeViewer.getTree().setEnabled(enabled);

    }
}
