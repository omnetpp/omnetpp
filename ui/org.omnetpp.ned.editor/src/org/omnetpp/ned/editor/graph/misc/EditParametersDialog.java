package org.omnetpp.ned.editor.graph.misc;

import java.util.List;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITableColorProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;

import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.properties.CheckboxControlCellEditor;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.ex.ParamNodeEx;
import org.omnetpp.ned.model.pojo.CommentNode;
import org.omnetpp.ned.model.pojo.NEDElementTags;


/**
 * Dialog for editing parameters in a module or channel.
 * @author rhornig
 */
public class EditParametersDialog extends TitleAreaDialog {
    private static final String COL_TYPE = "type";
    private static final String COL_NAME = "name";
    private static final String COL_DEFAULT = "def";
    private static final String COL_VALUE = "value";
    private static final String COL_COMMENT = "comment";
    private static final String VOLATILE = "volatile";

    private static final int ADD_ID = 500;
    private static final int DELETE_ID = 501;

	private final String title;
	private final List<ParamLine> params;
	// widgets
    private TableViewer listViewer;

	// sizing constants
    private final static int SIZING_SELECTION_WIDGET_HEIGHT = 114;
    private final static int SIZING_SELECTION_WIDGET_WIDTH = 480;

    private final class ColorTableLabelProvider extends TableLabelProvider implements ITableColorProvider {
        @Override
        public String getColumnText(Object element, int columnIndex) {
            ParamLine paramLine = (ParamLine)element;
            switch (columnIndex) {
                case 0: return paramLine.type;
                case 1: return paramLine.name;
                case 2: return paramLine.def ? "default" : "";
                case 3: return paramLine.value;
                case 4: return paramLine.comment;
            }
            return null;
        }

        @Override
        public Image getColumnImage(Object element, int columnIndex) {
            return null;
        }

        public Color getBackground(Object element, int columnIndex) {
            return null;
        }

        public Color getForeground(Object element, int columnIndex) {
            return columnIndex ==1 ? null : ColorFactory.RED ;
        }
    }

    public static class ParamLine {
        protected ParamNodeEx param;
        protected boolean inherited;
        protected boolean volat;
        protected boolean def;
        protected String type;
        protected String name;
        protected String value;
        protected String comment;

        public ParamLine(ParamNodeEx paramNode, boolean inherited) {
            this.inherited = inherited;
            param = paramNode;
            if (paramNode == null) {
                type = "int";
                name = "unknown";
                value ="";
                comment = "";
                volat = false;
                def  = false;
                return;
            }

            type = paramNode.getAttribute(ParamNodeEx.ATT_TYPE);
            if (paramNode.getIsVolatile())
                type = VOLATILE+" "+type;
            name = paramNode.getName();
            value = paramNode.getValue();
            def = paramNode.getIsDefault();
            comment = StringUtils.strip(StringUtils.removeStart(getComment(paramNode), "//"));
        }

        public ParamNodeEx getOriginalParamNode() {
            return param;
        }

        /**
         * @return The original node if no change occurred, or a new one with modified attributes
         */
        public ParamNodeEx getChangedParamNode() {
            ParamNodeEx result = null;

            if (param != null)
                result =  (ParamNodeEx)param.deepDup(null);
            else
                result = (ParamNodeEx)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_PARAM);

            // set the attributes here

            result.setName(name);
            result.setValue(value);
            result.setIsDefault(def);
            String baseType = StringUtils.strip(StringUtils.removeStart(StringUtils.strip(type), VOLATILE));
            result.setAttribute(ParamNodeEx.ATT_TYPE, baseType);
            result.setIsVolatile(StringUtils.strip(type).startsWith(VOLATILE));
            setComment(result, comment);

            return result;
        }

        /**
         * @param paramNode
         * @return The right comment of the parameter
         */
        protected String getComment(ParamNodeEx paramNode) { CommentNode cn = (CommentNode)paramNode.getFirstChildWithAttribute(NEDElementTags.NED_COMMENT, CommentNode.ATT_LOCID, "right");
            return cn == null ? null : cn.getContent().trim();
        }

        protected void setComment(ParamNodeEx paramNode, String comment) {
            // TODO support multi line comments and correct indentation
            String commentPadding = " // ";
            CommentNode cn = (CommentNode)paramNode.getFirstChildWithAttribute(NEDElementTags.NED_COMMENT, CommentNode.ATT_LOCID, "right");
            if (cn == null) {
                cn = (CommentNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementTags.NED_COMMENT, paramNode);
                cn.setLocid("right");
            } else {
                commentPadding = StringUtils.substringBefore(StringUtils.chomp(cn.getContent()), "//")+"// ";
            }
            cn.setContent(commentPadding + comment);
        }

    }

    /**
     * Creates the dialog.
     */
    public EditParametersDialog(Shell parentShell, List<ParamLine> params) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
        this.title = "Edit Parameters";
        this.params = params;
    }

    /**
     * @return The parameter line objects
     */
    public List<ParamLine> getParams() {
        return params;
    }

	@Override
    protected void configureShell(Shell shell) {
		super.configureShell(shell);
		if (title != null)
			shell.setText(title);
	}

    /* (non-Javadoc)
     * Method declared on Dialog.
     */
    @Override
    protected Control createDialogArea(Composite parent) {
        setTitle("Edit Parameters");
        setMessage("Add, remove or modify parameter type, name and value.");

        // page group
        Composite dialogArea = (Composite)super.createDialogArea(parent);

        Composite composite = new Composite(dialogArea, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(1,false));

		// table group
        Group group = new Group(composite, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		group.setText("Defined parameters");
		group.setLayout(new GridLayout(1, false));

        // table and buttons
		listViewer = createAndConfigureTable(group);
        GridData data = new GridData(GridData.FILL_BOTH);
        data.heightHint = SIZING_SELECTION_WIDGET_HEIGHT;
        data.widthHint = SIZING_SELECTION_WIDGET_WIDTH;
        listViewer.getTable().setLayoutData(data);

        addEditButtons(group);

        Dialog.applyDialogFont(composite);
        return composite;
    }

	protected Label createLabel(Composite composite, String text) {
		Label label = new Label(composite, SWT.NONE);
		label.setLayoutData(new GridData(SWT.END, SWT.BEGINNING, true, false));
		label.setText(text);
		return label;
	}

	/**
     * Add the selection and deselection buttons to the dialog.
     */
    private void addEditButtons(Composite composite) {
        Composite buttonComposite = new Composite(composite, SWT.NONE);
        GridLayout layout = new GridLayout();
        layout.numColumns = 0;
		layout.marginWidth = 0;
		layout.horizontalSpacing = convertHorizontalDLUsToPixels(IDialogConstants.HORIZONTAL_SPACING);
        buttonComposite.setLayout(layout);
        buttonComposite.setLayoutData(new GridData(SWT.END, SWT.TOP, true, false));

        Button button = createButton(buttonComposite, ADD_ID, "Add", false);
        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                addEntry();
            }
        });

        button = createButton(buttonComposite, DELETE_ID, "Delete", false);
        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                removeEntry();
            }
        });
    }

    protected void addEntry() {
        Object selObj = ((IStructuredSelection)listViewer.getSelection()).getFirstElement();
        ParamLine newParamLine = new ParamLine(null, false);
        params.add(params.indexOf(selObj)+1, newParamLine);
        listViewer.refresh();
        listViewer.setSelection(new StructuredSelection(newParamLine), true);
    }

	protected void removeEntry() {
	    Object selObj = ((IStructuredSelection)listViewer.getSelection()).getFirstElement();
	    params.remove(selObj);
        listViewer.refresh();
        selObj = ((IStructuredSelection)listViewer.getSelection()).getFirstElement();
        // check if we deleted the last parameter. in this case set the selection to the previous one

        if (selObj == null && params.size()>0)
            listViewer.setSelection(new StructuredSelection(params.get(params.size()-1)), true);
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
		createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
		createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
	}

    private TableColumn addTableColumn(Table table, String label, int width) {
        TableColumn column = new TableColumn(table, SWT.NONE);
        column.setText(label);
        column.setWidth(width);
        return column;
    }

    private TableViewer createAndConfigureTable(Composite parent) {
        Table table = new Table(parent, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION | SWT.VIRTUAL);
        table.setLinesVisible(true);
        table.setHeaderVisible(true);
        final TableViewer tableViewer = new TableViewer(table);
        addTableColumn(table, "Type", 80);
        addTableColumn(table, "Name", 100);
        addTableColumn(table, "Def", 50);
        addTableColumn(table, "Value", 120);
        addTableColumn(table, "Comment", 140);

        // set up tableViewer, content and label providers
        tableViewer.setContentProvider(new ArrayContentProvider());
        tableViewer.setLabelProvider(new ColorTableLabelProvider());

        // edit support
        tableViewer.setColumnProperties(new String[] {COL_TYPE, COL_NAME, COL_DEFAULT, COL_VALUE, COL_COMMENT});
        final CellEditor editors[] = new CellEditor[5];
        editors[0] = new TableTextCellEditor(tableViewer, 0, true);
        editors[1] = new TableTextCellEditor(tableViewer, 1, true);
        editors[2] = new CheckboxControlCellEditor(table);
        editors[3] = new TableTextCellEditor(tableViewer, 3, true);
        editors[4] = new TableTextCellEditor(tableViewer, 4, true);
        tableViewer.setCellEditors(editors);

        tableViewer.setCellModifier(new ICellModifier() {

            public boolean canModify(Object element, String property) {
                ParamLine paramLine = (ParamLine)element;
                // if it is an inherited property, then we cannot set/change the type and name
                if (paramLine.inherited && (COL_TYPE.equals(property) || COL_NAME.equals(property)))
                    return false;
                // otherwise we allow editing
                return true;
            }

            public Object getValue(Object element, String property) {
                ParamLine paramLine = (ParamLine)element;
                if (COL_TYPE.equals(property))
                    return paramLine.type;
                else if (COL_NAME.equals(property))
                    return paramLine.name;
                else if (COL_DEFAULT.equals(property))
                    return paramLine.def;
                else if (COL_VALUE.equals(property))
                    return paramLine.value;
                else if (COL_COMMENT.equals(property))
                    return paramLine.comment;
                return "";
            }

            public void modify(Object element, String property, Object value) {
                if (element instanceof Item)
                    element = ((Item) element).getData(); // workaround, see super's comment

                ParamLine paramLine = (ParamLine)element;

                if (COL_TYPE.equals(property)) {
                    paramLine.type = (String)value;
                } else if (COL_NAME.equals(property)) {
                    paramLine.name = (String)value;
                } else if (COL_DEFAULT.equals(property)) {
                    paramLine.def = (Boolean)value;
                } else if (COL_VALUE.equals(property)) {
                    paramLine.value = (String)value;
                } else if (COL_COMMENT.equals(property)) {
                    paramLine.comment = (String)value;
                }

                tableViewer.refresh(); // if performance gets critical: refresh only if changed
            }
        });

        tableViewer.setInput(params);
        return tableViewer;
    }

    @Override
	protected void okPressed() {
       super.okPressed();
    }

}
