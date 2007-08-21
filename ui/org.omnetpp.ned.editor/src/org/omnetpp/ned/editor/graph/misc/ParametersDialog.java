package org.omnetpp.ned.editor.graph.misc;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ObjectUtils;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITableColorProvider;
import org.eclipse.jface.viewers.ITableFontProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
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
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.editor.graph.commands.AddNEDElementCommand;
import org.omnetpp.ned.editor.graph.commands.DeleteCommand;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.ParamElement;
import org.omnetpp.ned.model.pojo.ParametersElement;


/**
 * Dialog for editing parameters in a module or channel.
 *
 * @author rhornig
 */
public class ParametersDialog extends TitleAreaDialog {
    private static final String COL_TYPE = "type";
    private static final String COL_NAME = "name";
    private static final String COL_VALUE = "value";
    private static final String COL_COMMENT = "comment";
    private static final String[] COLUMNS = new String[] {COL_TYPE, COL_NAME, COL_VALUE, COL_COMMENT};
    private static final String VOLATILE = "volatile";

    private static final int ADD_ID = 500;
    private static final int DELETE_ID = 501;

	private final String title;
	private final List<ParamLine> paramLines = new ArrayList<ParametersDialog.ParamLine>();
	// widgets
    private TableViewer listViewer;
    private Command resultCommand = UnexecutableCommand.INSTANCE;
    private final IHasParameters module;

	// sizing constants
    private final static int SIZING_SELECTION_WIDGET_HEIGHT = 114;
    private final static int SIZING_SELECTION_WIDGET_WIDTH = 480;

    private final class ParametersTableLabelProvider extends TableLabelProvider
                            implements ITableFontProvider, ITableColorProvider {
        @Override
        public String getColumnText(Object element, int columnIndex) {
            ParamLine paramLine = (ParamLine)element;
            switch (columnIndex) {
                case 0: return paramLine.type.value;
                case 1: return paramLine.name.value;
                case 2: return paramLine.value.value;
                case 3: return paramLine.comment.value;
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
            ParamLine paramLine = (ParamLine)element;
            boolean isEditable = false;
            switch (columnIndex) {
                case 0: isEditable = paramLine.type.isEditable; break;
                case 1: isEditable = paramLine.name.isEditable; break;
                case 2: isEditable = paramLine.value.isEditable; break;
                case 3: isEditable = paramLine.comment.isEditable; break;
            }
            return isEditable ? null : ColorFactory.GREY50;
        }

        public Font getFont(Object element, int columnIndex) {
            ParamLine paramLine = (ParamLine)element;
            boolean isInherited = false;
            switch (columnIndex) {
                case 0: isInherited = paramLine.type.isItalic; break;
                case 1: isInherited = paramLine.name.isItalic; break;
                case 2: isInherited = paramLine.value.isItalic; break;
                case 3: isInherited = paramLine.comment.isItalic; break;
            }
            return isInherited ? JFaceResources.getFontRegistry().getItalic(JFaceResources.DIALOG_FONT) : null;
        }
    }


    protected static class ParamLine {
        protected static class Cell {
            String value;
            String originalValue;
            boolean isEditable = true;
            boolean isItalic = false;
            boolean isDirty() {
                return !ObjectUtils.equals(value, originalValue);
            }
        }
        protected ParamElementEx param;
        protected Cell type = new Cell();
        protected Cell name = new Cell();
        protected Cell value = new Cell();
        protected Cell comment = new Cell();

        public ParamLine(ParamElementEx paramNode) {
            param = paramNode;
            if (paramNode == null) {
                type.value = "int";
                name.value = "unknown";
                value.value ="";
                comment.value = "";
                return;
            }
        }

        public ParamElementEx getOriginalParamNode() {
            return param;
        }

        /**
         * Returns the original node if no change occurred, or a new one with modified attributes
         */
        public ParamElementEx getChangedParamNode() {
            ParamElementEx result = null;

            // do not include a value assignment for this node if no value or comment was given
            if (StringUtils.isEmpty(value.value) && StringUtils.isEmpty(comment.value) && !type.isEditable ||
                    !value.isDirty() && !comment.isDirty())
                return null;

            if (param != null)
                result =  (ParamElementEx)param.deepDup();
            else
                result = (ParamElementEx)NEDElementFactoryEx.getInstance().createElement(NEDElementFactoryEx.NED_PARAM);

            // set the attributes here

            result.setName(name.value);
            result.setValue(value.value);
            setComment(result, comment.value);
            if (type.isEditable) {
                String baseType = StringUtils.strip(StringUtils.removeStart(StringUtils.strip(type.value), VOLATILE));
                result.setAttribute(ParamElementEx.ATT_TYPE, baseType);
                result.setIsVolatile(StringUtils.strip(type.value).startsWith(VOLATILE));
            }
            else {
                result.setAttribute(ParamElementEx.ATT_TYPE, "");
                result.setIsVolatile(false);
            }
            return result;
        }

        protected void setComment(ParamElementEx paramNode, String comment) {
            // TODO support multi line comments and correct indentation
            String commentPadding = " // ";
            CommentElement cn = (CommentElement)paramNode.getFirstChildWithAttribute(NEDElementTags.NED_COMMENT, CommentElement.ATT_LOCID, "right");
            if (cn == null) {
                cn = (CommentElement)NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_COMMENT, paramNode);
                cn.setLocid("right");
            }
            else {
                commentPadding = StringUtils.substringBefore(StringUtils.chomp(cn.getContent()), "//")+"// ";
            }
            if (StringUtils.strip(comment).equals(""))
                cn.setContent("");
            else
                cn.setContent(commentPadding + comment);
        }

    }

    /**
     * Creates the dialog.
     */
    public ParametersDialog(Shell parentShell, IHasParameters module) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
        this.title = "Edit Parameters";
        this.module = module;
        buildTable();
    }

    protected void buildTable() {
        // build ParamLine list (value objects) for dialog editing
        for (ParamElement paramDecl : module.getParamDeclarations().values()) {
            // get the the value of the parameter
            ParamElementEx paramValue = (ParamElementEx)module.getParamAssignments().get(paramDecl.getName());
            ParamLine paramLine = new ParamLine(paramValue);
            boolean isDeclLocal = paramDecl.getParent().getParent() == module;
            boolean isValueLocal = paramValue != null && paramValue.getParent().getParent() == module;
            paramLine.type.isItalic = !isDeclLocal;
            paramLine.type.isEditable = isDeclLocal;
            paramLine.name.isItalic = !isDeclLocal && !isValueLocal;
            paramLine.name.isEditable = isDeclLocal;
            paramLine.value.isItalic = paramLine.comment.isItalic = !isValueLocal;
            paramLine.value.isEditable = paramLine.comment.isEditable = true;

            // fill the values
            paramLine.type.value = paramLine.type.originalValue =
                                        (paramDecl.getIsVolatile() ? VOLATILE+" " : "")
                                        + paramDecl.getAttribute(ParamElementEx.ATT_TYPE);
            paramLine.name.value = paramLine.name.originalValue = paramDecl.getName();
            paramLine.value.value = paramLine.value.originalValue = paramValue.getValue();
            // FIXME remove // from each line
            paramLine.comment.value = paramLine.comment.originalValue =
                StringUtils.strip(StringUtils.removeStart(getComment(paramValue), "//"));

            paramLines.add(paramLine);
        }
    }

    /**
     * Returns the comment of the parameter; the "right-comment" is used.
     */
    protected static String getComment(ParamElementEx paramNode) {
        CommentElement cn = (CommentElement)paramNode.getFirstChildWithAttribute(NEDElementTags.NED_COMMENT, CommentElement.ATT_LOCID, "right");
        return cn == null ? "" : cn.getContent().trim();
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
        ParamLine newParamLine = new ParamLine(null);
        paramLines.add(paramLines.indexOf(selObj)+1, newParamLine);
        listViewer.refresh();
        listViewer.setSelection(new StructuredSelection(newParamLine), true);
    }

	protected void removeEntry() {
	    Object selObj = ((IStructuredSelection)listViewer.getSelection()).getFirstElement();
	    paramLines.remove(selObj);
        listViewer.refresh();
        selObj = ((IStructuredSelection)listViewer.getSelection()).getFirstElement();
        // check if we deleted the last parameter. in this case set the selection to the previous one

        if (selObj == null && paramLines.size()>0)
            listViewer.setSelection(new StructuredSelection(paramLines.get(paramLines.size()-1)), true);
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
        addTableColumn(table, "Value", 120);
        addTableColumn(table, "Comment", 140);

        // set up tableViewer, content and label providers
        tableViewer.setContentProvider(new ArrayContentProvider());
        tableViewer.setLabelProvider(new ParametersTableLabelProvider());

        // edit support
        tableViewer.setColumnProperties(COLUMNS);
        final CellEditor editors[] = new CellEditor[] {
            new TableTextCellEditor(tableViewer, 0, true),
            new TableTextCellEditor(tableViewer, 1, true),
            new TableTextCellEditor(tableViewer, 2, true),
            new TableTextCellEditor(tableViewer, 3, true)
        };
        tableViewer.setCellEditors(editors);

        tableViewer.setCellModifier(new ICellModifier() {

            public boolean canModify(Object element, String property) {
                ParamLine paramLine = (ParamLine)element;
                if (COL_TYPE.equals(property))
                    return paramLine.type.isEditable;
                else if (COL_NAME.equals(property))
                    return paramLine.name.isEditable;
                else if (COL_VALUE.equals(property))
                    return paramLine.value.isEditable;
                else if (COL_COMMENT.equals(property))
                    return paramLine.comment.isEditable;
                return false;
            }

            public Object getValue(Object element, String property) {
                ParamLine paramLine = (ParamLine)element;
                if (COL_TYPE.equals(property))
                    return paramLine.type.value;
                else if (COL_NAME.equals(property))
                    return paramLine.name.value;
                else if (COL_VALUE.equals(property))
                    return paramLine.value.value;
                else if (COL_COMMENT.equals(property))
                    return paramLine.comment.value;
                return "";
            }

            public void modify(Object element, String property, Object value) {
                if (element instanceof Item)
                    element = ((Item) element).getData(); // workaround, see super's comment

                ParamLine paramLine = (ParamLine)element;

                if (COL_TYPE.equals(property)) {
                    paramLine.type.value = (String)value;
                }
                else if (COL_NAME.equals(property)) {
                    paramLine.name.value = (String)value;
                }
                else if (COL_VALUE.equals(property)) {
                    paramLine.value.value = (String)value;
                }
                else if (COL_COMMENT.equals(property)) {
                    paramLine.comment.value = (String)value;
                }

                tableViewer.refresh(); // if performance gets critical: refresh only if changed
            }
        });

        tableViewer.setInput(paramLines);
        return tableViewer;
    }

    @Override
	protected void okPressed() {
       INEDElement paramsNode = module.getFirstChildWithTag(NEDElementTags.NED_PARAMETERS);
       ParametersElement newParamsNode = paramsNode != null ? (ParametersElement)paramsNode.deepDup() :
                                   (ParametersElement)NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_PARAMETERS);

       for (INEDElement paramNode : newParamsNode)
           if (paramNode instanceof ParamElementEx)
               paramNode.removeFromParent();

       // add the new nodes
       for (ParametersDialog.ParamLine paramLine : paramLines) {
           ParamElementEx param = paramLine.getChangedParamNode();
           if (param != null)
               newParamsNode.appendChild(param);
       }

       // create a replace / compound command
       CompoundCommand paramReplaceCommand = new CompoundCommand("Change Parameters");
       if (paramsNode != null)
           paramReplaceCommand.add(new DeleteCommand(paramsNode));

       paramReplaceCommand.add(new AddNEDElementCommand(module, newParamsNode));
       resultCommand = paramReplaceCommand;
       super.okPressed();
    }

    public Command getResultCommand() {
        return resultCommand;
    }

}
