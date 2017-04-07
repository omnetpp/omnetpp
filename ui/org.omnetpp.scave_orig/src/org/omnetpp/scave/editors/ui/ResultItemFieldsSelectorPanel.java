package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.omnetpp.scave.engine.ResultItemField;

import static org.omnetpp.scave.engine.ResultItemField.*;

/**
 *
 *
 * @author tomi
 */
public class ResultItemFieldsSelectorPanel extends Composite {

    private List<ResultItemField> selectableFields;
    private Tree tree;
    private List<TreeItem> treeItems;

    public ResultItemFieldsSelectorPanel(Composite parent, int style) {
        super(parent, style);
        setLayout(new GridLayout());
    }

    public void setSelectableResultItemFields(List<ResultItemField> fields) {
        Assert.isNotNull(fields);
        if (!fields.equals(selectableFields)) {
            this.selectableFields = fields;
            if (tree != null)
                tree.dispose();
            tree = createTree();
        }
    }

    public void setSelectedResultItemFields(List<ResultItemField> data) {
        if (tree != null) {
            for (TreeItem item : treeItems) {
                item.setChecked(data.contains(item.getData()));
            }
        }
    }

    public void setSelectedResultItemFieldsByName(List<String> names) {
        if (tree != null) {
            for (TreeItem item : treeItems) {
                ResultItemField field = (ResultItemField)item.getData();
                item.setChecked(names.contains(field.getName()));
            }
        }
    }

    public List<ResultItemField> getSelectedResultItemFields() {
        List<ResultItemField> fields = new ArrayList<ResultItemField>();
        if (tree != null) {
            for (TreeItem item : treeItems)
                if (item.getChecked())
                    fields.add((ResultItemField)item.getData());
        }
        return fields;
    }

    public List<String> getSelectedResultItemFieldNames() {
        List<String> names = new ArrayList<String>();
        if (tree != null) {
            for (TreeItem item : treeItems) {
                if (item.getChecked()) {
                    ResultItemField field = (ResultItemField)item.getData();
                    names.add(field.getName());
                }
            }
        }
        return names;
    }

    private Tree createTree() {
        tree = new Tree(getParent(),
                SWT.BORDER | SWT.CHECK | SWT.HIDE_SELECTION | SWT.V_SCROLL);
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        int itemsVisible = Math.max(Math.min(selectableFields.size(), 15), 3);
        gridData.heightHint = itemsVisible * tree.getItemHeight();
        tree.setLayoutData(gridData);
        treeItems = new ArrayList<TreeItem>();
        createTreeItems("general", -1, true);
        createTreeItems("run attributes", RUN_ATTR_ID, false);
        createTreeItems("module parameters", RUN_PARAM_ID, false);
        return tree;
    }

    private TreeItem createTreeItems(String name, int fieldID, boolean expanded) {
        List<ResultItemField> fields = filterFieldsByID(selectableFields, fieldID);
        if (fields.isEmpty())
            return null;

        TreeItem toplevelItem = new TreeItem(tree, SWT.NONE);
        toplevelItem.setText(name);
        for (ResultItemField field : fields) {
            TreeItem treeItem = new TreeItem(toplevelItem, SWT.NONE);
            treeItem.setChecked(false);
            treeItem.setText(field.getName());
            treeItem.setData(field);
            treeItems.add(treeItem);
        }
        toplevelItem.setExpanded(expanded);
        return toplevelItem;
    }

    private static List<ResultItemField> filterFieldsByID(List<ResultItemField> fields, int fieldID) {
        List<ResultItemField> result = new ArrayList<ResultItemField>();
        for (ResultItemField field : fields) {
            int id = field.getID();
            if (id == fieldID || (fieldID == -1 && (id == FILE_ID || id == RUN_ID ||
                                                    id == MODULE_ID ||  id == NAME_ID)))
                result.add(field);
        }
        return result;
    }
}
