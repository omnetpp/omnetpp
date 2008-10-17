package org.omnetpp.ned.editor.graph.misc;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.ObjectUtils;
import org.apache.commons.lang.StringEscapeUtils;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITableColorProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
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
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.properties.EnumCellEditor;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.editor.graph.commands.AddNEDElementCommand;
import org.omnetpp.ned.editor.graph.commands.DeleteCommand;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.interfaces.IInterfaceTypeElement;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.ParametersElement;


/**
 * Dialog for editing parameters in a module or channel.
 *
 * @author rhornig, levy
 */
// used from ParametersPropertySource and ParametersDialogAction
public class ParametersDialog extends TitleAreaDialog {
    // constants
    // TODO: create icons
    private static final String IMAGE_INHERITED_DECLARATION = ImageFactory.DECORATOR_IMAGE_WARNING;
    private static final String IMAGE_OVERRIDDEN_DECLARATION = ImageFactory.DECORATOR_IMAGE_ERROR;
    private static final String IMAGE_LOCAL_DECLARATION = ImageFactory.CURSOR_IMAGE_ZOOMIN;

    private static final String COLUMN_TYPE = "type";
    private static final String COLUMN_NAME = "name";
    private static final String COLUMN_VALUE = "value";
    private static final String COLUMN_COMMENT = "comment";
    private static final String[] COLUMNS = new String[] {COLUMN_TYPE, COLUMN_NAME, COLUMN_VALUE, COLUMN_COMMENT};
    private static final String[] INTERFACE_TYPE_COLUMNS = new String[] {COLUMN_TYPE, COLUMN_NAME, COLUMN_COMMENT};
    private static final String[] TYPES = new String[] {"bool", "int", "double", "string", "xml", "volatile bool", "volatile int", "volatile double", "volatile string", "volatile xml"};
    private static final String DEFAULT_TYPE = "int";
    private static final String VOLATILE_PARAMETER_PREFIX = "volatile";
    private static final String DEFAULT_VALUE_PREFIX = "default(";
    private static final String DEFAULT_VALUE_SUFFIX = ")";

    private static final int BUTTON_ADD_ID = 500;
    private static final int BUTTON_REMOVE_ID = 501;

	private final String dialogTitle;
	private final List<ParamLine> paramLines;

	// widgets
    private TableViewer listViewer;
    private Command resultCommand = UnexecutableCommand.INSTANCE;
    private final IHasParameters parameterProvider;

	// sizing constants
    private final static int SIZING_SELECTION_WIDGET_HEIGHT = 200;
    private final static int SIZING_SELECTION_WIDGET_WIDTH = 640;

    private final class ParametersTableLabelProvider extends TableLabelProvider implements ITableColorProvider
    {
        @Override
        public String getColumnText(Object element, int columnIndex) {
            ParamLine paramLine = (ParamLine)element;

            switch (columnIndex) {
                case 0: return paramLine.type; 
                case 1: return paramLine.name; 
                case 2: 
                    if (parameterProvider instanceof IInterfaceTypeElement)
                        return paramLine.comment;
                    else
                        return paramLine.value;
                case 3: return paramLine.comment;
                default:
                    throw new RuntimeException();
            }
        }

        @Override
        public Image getColumnImage(Object element, int columnIndex) {
            if (columnIndex == 0) {
                ParamLine paramLine = (ParamLine)element;

                if (paramLine.isOriginallyLocalDeclaration())
                    return ImageFactory.getImage(IMAGE_LOCAL_DECLARATION);
                else if (paramLine.isCurrentlyOverridden())
                    return ImageFactory.getImage(IMAGE_OVERRIDDEN_DECLARATION);
                else
                    return ImageFactory.getImage(IMAGE_INHERITED_DECLARATION);
            }
            else
                return null;
        }

        public Color getBackground(Object element, int columnIndex) {
            return null;
        }

        public Color getForeground(Object element, int columnIndex) {
            ParamLine paramLine = (ParamLine)element;

            if ((columnIndex == 0 || columnIndex == 1) && !paramLine.isOriginallyLocalDeclaration()) 
                return ColorFactory.GREY50;
            else
                return null;
        }
    }

    protected class ParamLine {
        // original elements in the model, corresponding functions return values without considering the current state
        private ParamElementEx originalParamDeclaration; // the end of the extends chain where the parameter is declared
        private List<ParamElementEx> originalParamInheritanceChain; // the whole inheritance chain except the local parameter
        private ParamElementEx originalParamLocal; // the beginning of the extends chain directly under parameterProvider or null

        // current state
        public ParamElementEx currentParam; // the current parameter to be replaced or inserted
        public String type;
        public String name;
        public String value;
        public String comment;

        public ParamLine() {
            this(new ArrayList<ParamElementEx>());
            this.type = DEFAULT_TYPE;
            
            for (int i = 0;; i++) {
                String name = "unnamed" + i;

                if (getParamLine(name) != null)
                    continue;
                
                this.name = name;
                break;
            }

            updateCurrentParam();
        }

        public ParamLine(List<ParamElementEx> originalParamInheritanceChain) {
            this.originalParamInheritanceChain = new ArrayList<ParamElementEx>(originalParamInheritanceChain);
            
            int size = originalParamInheritanceChain.size();
            if (size == 0) {
                this.originalParamDeclaration = null;
                this.originalParamLocal = null;
            }
            else {
                this.originalParamDeclaration = originalParamInheritanceChain.get(size - 1);
                ParamElementEx paramElement = originalParamInheritanceChain.get(0);
                
                if (paramElement.getParent().getParent() == parameterProvider)
                    this.originalParamLocal = originalParamInheritanceChain.get(0);
                else
                    this.originalParamLocal = null;
                
                if (originalParamLocal != null)
                    this.originalParamInheritanceChain.remove(originalParamLocal);
            }

            resetToOriginal();
        }

        public void resetToOriginal() {
            this.type = getOriginalType();
            this.name = getOriginalName();
            this.value = getOriginalValue();
            this.comment = getOriginalComment();
            this.currentParam = originalParamLocal;
        }

        public void resetToCurrent() {
            this.type = getOriginalType();
            this.name = getOriginalName();
            this.value = getCurrentValue();
            this.comment = getCurrentComment();
        }

        public String getOriginalType() {
            return originalParamDeclaration == null ? "" : getType(originalParamDeclaration);
        }

        public String getOriginalName() {
            return originalParamDeclaration == null ? "" : getName(originalParamDeclaration);
        }

        public String getOriginalValue() {
            return originalParamLocal == null ? getValue(originalParamInheritanceChain) : getValue(originalParamLocal);
        }

        public String getOriginalComment() {
            return originalParamLocal == null ? getComment(originalParamInheritanceChain) : getComment(originalParamLocal);
        }

        public boolean isOriginallyLocalDeclaration() {
            return originalParamDeclaration == originalParamLocal;
        }
        
        public boolean isOriginallyOverridden() {
            return !isOriginallyLocalDeclaration() && originalParamLocal != originalParamDeclaration;
        }

        public String getCurrentValue() {
            return currentParam == null ? getValue(originalParamInheritanceChain) : getValue(currentParam);
        }

        public String getCurrentComment() {
            return currentParam == null ? getComment(originalParamInheritanceChain) : getComment(currentParam);
        }

        public boolean isCurrentlyOverridden() {
            return !isOriginallyLocalDeclaration() && currentParam != null;
        }

        public boolean isDirtyValue() {
            return !ObjectUtils.equals(type, getOriginalType()) || !ObjectUtils.equals(name, getOriginalName()) ||
                   !ObjectUtils.equals(value, getOriginalValue()) || !ObjectUtils.equals(comment, getOriginalComment());
        }

        public void updateCurrentParam() {
            if (!isDirtyValue())
                currentParam = originalParamLocal;
            else {
                if (originalParamLocal != null)
                    currentParam = (ParamElementEx)originalParamLocal.deepDup();
                else
                    currentParam = (ParamElementEx)NEDElementFactoryEx.getInstance().createElement(NEDElementFactoryEx.NED_PARAM);
    
                setParamAttributes(currentParam);
            }
        }

        private void setParamAttributes(ParamElementEx paramElement) {
            setType(paramElement, type);
            setName(paramElement, name);
            setValue(paramElement, value);
            setComment(paramElement, comment);
        }
        
        protected String getType(ParamElementEx paramElement) {
            return (paramElement.getIsVolatile() ? VOLATILE_PARAMETER_PREFIX + " " : "") + paramElement.getAttribute(ParamElementEx.ATT_TYPE);
        }
        
        protected void setType(ParamElementEx paramElement, String type) {
            if (isOriginallyLocalDeclaration()) {
                String baseType = StringUtils.strip(StringUtils.removeStart(StringUtils.strip(type), VOLATILE_PARAMETER_PREFIX));
                paramElement.setAttribute(ParamElementEx.ATT_TYPE, baseType);
                paramElement.setIsVolatile(StringUtils.strip(type).startsWith(VOLATILE_PARAMETER_PREFIX));
            }
            else {
                paramElement.setAttribute(ParamElementEx.ATT_TYPE, "");
                paramElement.setIsVolatile(false);
            }
        }
        
        protected String getName(ParamElementEx paramElement) {
            return paramElement.getName();
        }
        
        protected void setName(ParamElementEx paramElement, String name) {
            paramElement.setName(name);
        }
        
        protected String getValue(List<ParamElementEx> originalParamInheritanceChain2) {
            for (ParamElementEx paramElement : originalParamInheritanceChain2) {
                String value = getValue(paramElement);
                
                if (!StringUtils.isEmpty(value))
                    return value;
            }

            return "";
        }

        protected String getValue(ParamElementEx paramElement) {
            if (paramElement.getIsDefault())
                return DEFAULT_VALUE_PREFIX + paramElement.getValue() + DEFAULT_VALUE_SUFFIX;
            else
                return paramElement.getValue();
        }
        
        protected void setValue(ParamElementEx paramElement, String value) {
            String strippedValue = StringUtils.strip(value);
            paramElement.setIsDefault(strippedValue.startsWith(DEFAULT_VALUE_PREFIX) && strippedValue.endsWith(DEFAULT_VALUE_SUFFIX));
            paramElement.setValue(StringUtils.removeEnd(StringUtils.removeStart(strippedValue, DEFAULT_VALUE_PREFIX), DEFAULT_VALUE_SUFFIX));
        }

        protected String getComment(List<ParamElementEx> originalParamInheritanceChain2) {
            for (ParamElementEx paramElement : originalParamInheritanceChain2) {
                String comment = getComment(paramElement);
                
                if (!StringUtils.isEmpty(comment))
                    return comment;
            }

            return "";
        }

        /**
         * Returns the comment of the parameter; the "right-comment" is used.
         */
        protected String getComment(ParamElementEx paramElement) {
            CommentElement commentElement = (CommentElement)paramElement.getFirstChildWithAttribute(NEDElementTags.NED_COMMENT, CommentElement.ATT_LOCID, "right");

            if (commentElement == null) 
                return "";
            else {
                String comment = StringUtils.strip(commentElement.getContent());
                String[] lines = StringUtils.splitToLines(comment);

                for (int i = 0; i < lines.length; i++)
                    lines[i] = StringUtils.strip(StringUtils.removeStart(StringUtils.strip(lines[i]), "//"));

                return StringEscapeUtils.escapeJava(StringUtils.join(lines, "\n"));
            }
        }

        protected void setComment(ParamElementEx paramElement, String comment) {
            String commentPadding = " // ";
            CommentElement commentElement = (CommentElement)paramElement.getFirstChildWithAttribute(NEDElementTags.NED_COMMENT, CommentElement.ATT_LOCID, "right");
            
            if (commentElement == null) {
                commentElement = (CommentElement)NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_COMMENT, paramElement);
                commentElement.setLocid("right");
            }
            else
                commentPadding = StringUtils.substringBefore(StringUtils.chomp(commentElement.getContent()), "//") + "// ";

            if (StringUtils.strip(comment).equals(""))
                commentElement.setContent("");
            else {
                if (originalParamLocal != null) {
                    String indent = StringUtils.repeat(" ", originalParamLocal.getSourceRegion().getEndColumn());
                    commentPadding = indent + commentPadding;
                }

                comment = StringUtils.indentLines(StringEscapeUtils.unescapeJava(comment), commentPadding);
                commentElement.setContent(" " + StringUtils.strip(comment));
            }
        }
    }
    
    public ParamLine getParamLine(String name) {
        for (ParamLine paramLine : paramLines)
            if (name.equals(paramLine.name))
                return paramLine;
        
        return null;
    }

    /**
     * Creates the dialog.
     */
    public ParametersDialog(Shell parentShell, IHasParameters parameterProvider) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
        this.dialogTitle = "Edit Parameters";
        this.parameterProvider = parameterProvider;
        paramLines = collectParamLines();
    }

    protected List<ParamLine> collectParamLines() {
        List<ParamLine> params = new ArrayList<ParamLine>();
        Map<String, ParamElementEx> paramDeclarations = parameterProvider.getParamDeclarations(); 
        Map<String, ParamElementEx> paramAssignments = parameterProvider.getParamAssignments(); 

        // build ParamLine list (value objects) for dialog editing
        for (ParamElementEx paramDeclaration : paramDeclarations.values()) {
            String parameterName = paramDeclaration.getName();
            params.add(new ParamLine(parameterProvider.getParameterInheritanceChain(parameterName)));
        }

        // add those assignments which do not have corresponding declarations
        for (ParamElementEx paramValue : paramAssignments.values()) {
            String parameterName = paramValue.getName();

            if (!paramDeclarations.containsKey(paramValue.getName()))
                params.add(new ParamLine(parameterProvider.getParameterInheritanceChain(parameterName)));
        }

        return params;
    }

	@Override
    protected void configureShell(Shell shell) {
		super.configureShell(shell);
		if (dialogTitle != null)
			shell.setText(dialogTitle);
	}

    /* (non-Javadoc)
     * Method declared on Dialog.
     */
    @Override
    protected Control createDialogArea(Composite parent) {
        setTitle(dialogTitle);
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

    private void addEditButtons(Composite composite) {
        Composite buttonComposite = new Composite(composite, SWT.NONE);
        GridLayout layout = new GridLayout();
        layout.numColumns = 0;
		layout.marginWidth = 0;
		layout.horizontalSpacing = convertHorizontalDLUsToPixels(IDialogConstants.HORIZONTAL_SPACING);
        buttonComposite.setLayout(layout);
        buttonComposite.setLayoutData(new GridData(SWT.END, SWT.TOP, true, false));

        // add button
        Button addButton = createButton(buttonComposite, BUTTON_ADD_ID, "Add", false);
        addButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                Object selected = ((IStructuredSelection)listViewer.getSelection()).getFirstElement();
                ParamLine newParamLine = new ParamLine();
                paramLines.add(paramLines.indexOf(selected) + 1, newParamLine);
                listViewer.refresh();
                listViewer.setSelection(new StructuredSelection(newParamLine), true);
            }
        });
        addButton.setEnabled(!(parameterProvider instanceof SubmoduleElementEx));

        // remove button
        final Button removeButton = createButton(buttonComposite, BUTTON_REMOVE_ID, "Remove", false);
        removeButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent event) {
                int index = -1;
                ISelection selection = listViewer.getSelection();
                StructuredSelection structuredSelection = (StructuredSelection)selection;

                for (Object element : structuredSelection.toList()) {
                    ParamLine paramLine = (ParamLine)element;
                    
                    if (index == -1)
                        index = paramLines.indexOf(paramLine);
                    
                    if (paramLine.isOriginallyLocalDeclaration())
                        paramLines.remove(paramLine);
                    else if (paramLine.isOriginallyOverridden()) {
                        paramLine.currentParam = null;
                        paramLine.resetToCurrent();
                    }
                    else if (paramLine.isCurrentlyOverridden())
                        paramLine.resetToOriginal();
                    else
                        throw new RuntimeException();
                }

                if (paramLines.size() != 0)
                    listViewer.setSelection(new StructuredSelection(index), true);

                listViewer.refresh();
            }
        });
        listViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                ISelection selection = event.getSelection();
                StructuredSelection structuredSelection = (StructuredSelection)selection;

                for (Object element : structuredSelection.toList()) {
                    ParamLine paramLine = (ParamLine)element;
                    if (!paramLine.isCurrentlyOverridden() && !paramLine.isOriginallyLocalDeclaration()) {
                        removeButton.setEnabled(false);
                        return;
                    }
                }

                removeButton.setEnabled(true);
            }
        });
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
		createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
		createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
	}

    private TableColumn addTableColumn(final Table table, String label, int width) {
        final TableColumn column = new TableColumn(table, SWT.NONE);
        column.setText(label);
        
        if (width != -1)
            column.setWidth(width);
        else {
            table.addControlListener(new ControlListener() {
                public void controlMoved(ControlEvent e) {
                }

                public void controlResized(ControlEvent e) {
                    int totalWidth = 0;

                    for (TableColumn otherColumn : table.getColumns())
                        if (!column.equals(otherColumn))
                            totalWidth += otherColumn.getWidth();
                    
                    column.setWidth(table.getSize().x - totalWidth - 2 * table.getBorderWidth() - 20); // image size
                }
            });
        }

        return column;
    }

    private TableViewer createAndConfigureTable(Composite parent) {
        Table table = new Table(parent, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION | SWT.VIRTUAL);
        table.setLinesVisible(true);
        table.setHeaderVisible(true);

        addTableColumn(table, "Type", 100);
        addTableColumn(table, "Name", 100);
        if (!(parameterProvider instanceof IInterfaceTypeElement))
            addTableColumn(table, "Value", 120);
        addTableColumn(table, "Comment", -1);

        // set up tableViewer, content and label providers
        final TableViewer tableViewer = new TableViewer(table);
        tableViewer.setContentProvider(new ArrayContentProvider());
        tableViewer.setLabelProvider(new ParametersTableLabelProvider());

        // edit support
        final CellEditor[] editors;
        if (parameterProvider instanceof IInterfaceTypeElement) {
            tableViewer.setColumnProperties(INTERFACE_TYPE_COLUMNS);
            editors = new CellEditor[] {
                new EnumCellEditor(table, TYPES, TYPES),
                new TableTextCellEditor(tableViewer, 1),
                new TableTextCellEditor(tableViewer, 2)
            };
        }
        else {
            tableViewer.setColumnProperties(COLUMNS);
            editors = new CellEditor[] {
                new EnumCellEditor(table, TYPES, TYPES),
                new TableTextCellEditor(tableViewer, 1),
                new TableTextCellEditor(tableViewer, 2),
                new TableTextCellEditor(tableViewer, 3)
            };
        }
        tableViewer.setCellEditors(editors);

        tableViewer.setCellModifier(new ICellModifier() {
            public boolean canModify(Object element, String property) {
                ParamLine paramLine = (ParamLine)element;
                
                if (COLUMN_TYPE.equals(property))
                    return paramLine.isOriginallyLocalDeclaration();
                else if (COLUMN_NAME.equals(property))
                    return paramLine.isOriginallyLocalDeclaration();
                else if (COLUMN_VALUE.equals(property))
                    return true;
                else if (COLUMN_COMMENT.equals(property))
                    return true;
                else
                    throw new RuntimeException();
            }

            public Object getValue(Object element, String property) {
                ParamLine paramLine = (ParamLine)element;

                if (COLUMN_TYPE.equals(property))
                    return paramLine.type;
                else if (COLUMN_NAME.equals(property))
                    return paramLine.name;
                else if (COLUMN_VALUE.equals(property))
                    return paramLine.value;
                else if (COLUMN_COMMENT.equals(property))
                    return paramLine.comment;
                else
                    throw new RuntimeException();
            }

            public void modify(Object element, String property, Object value) {
                if (element instanceof Item)
                    element = ((Item)element).getData(); // workaround, see super's comment

                ParamLine paramLine = (ParamLine)element;
                String oldValue;
                String newValue = (String)value;

                if (COLUMN_TYPE.equals(property)) {
                    oldValue = paramLine.type;
                    paramLine.type = newValue;
                }
                else if (COLUMN_NAME.equals(property)) {
                    oldValue = paramLine.name;
                    paramLine.name = newValue;
                }
                else if (COLUMN_VALUE.equals(property)) {
                    oldValue = paramLine.value;
                    paramLine.value = newValue;
                }
                else if (COLUMN_COMMENT.equals(property)) {
                    oldValue = paramLine.comment;
                    paramLine.comment = newValue;
                }
                else
                    throw new RuntimeException();

                if (!ObjectUtils.equals(oldValue, newValue))
                    paramLine.updateCurrentParam();

                tableViewer.refresh(); // if performance gets critical: refresh only if changed
            }
        });

        tableViewer.setInput(paramLines);
        return tableViewer;
    }

    @Override
	protected void okPressed() {
       INEDElement parametersElement = parameterProvider.getFirstChildWithTag(NEDElementTags.NED_PARAMETERS);
       ParametersElement newParametersElement = parametersElement != null ? (ParametersElement)parametersElement.deepDup() :
           (ParametersElement)NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_PARAMETERS);

       // remove old parameters from copy
       for (INEDElement element : newParametersElement)
           if (element instanceof ParamElementEx)
               element.removeFromParent();

       // add the new parameters to copy
       for (ParamLine paramLine : paramLines)
           if (paramLine.currentParam != null)
               newParametersElement.appendChild(paramLine.currentParam);

       // create a compound replace command
       CompoundCommand parameterReplaceCommand = new CompoundCommand("Change Parameters");
       if (parametersElement != null)
           parameterReplaceCommand.add(new DeleteCommand(parametersElement));

       if (newParametersElement.getFirstChild() != null)
           parameterReplaceCommand.add(new AddNEDElementCommand(parameterProvider, newParametersElement));
       else if (parametersElement != null)
           parameterReplaceCommand.add(new DeleteCommand(parametersElement));

       resultCommand = parameterReplaceCommand;
       super.okPressed();
    }

    public Command getResultCommand() {
        return resultCommand;
    }
}
