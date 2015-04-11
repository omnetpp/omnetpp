/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.views;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.WeakHashMap;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerFilter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.TextTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.util.ActionExt;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.IGotoInifile;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.ITimeout;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.IAnalysisListener;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamCollector;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.ParamResolution.ParamResolutionType;
import org.omnetpp.inifile.editor.model.ParamResolutionDisabledException;
import org.omnetpp.inifile.editor.model.ParamResolutionTimeoutException;
import org.omnetpp.inifile.editor.model.SectionKey;
import org.omnetpp.inifile.editor.model.Timeout;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;


/**
 * Displays module parameters recursively for a module type.
 * @author Andras
 */
public class ModuleParametersView extends AbstractModuleView {
    private TableViewer tableViewer;
    private boolean unassignedOnly = false;
    private TableColumn parameterColumn;
    private TableColumn valueColumn;
    private TableColumn remarkColumn;
    private IInifileDocument inifileDocument; // corresponds to the current selection; unfortunately needed by the label provider
    private InifileAnalyzer inifileAnalyzer; // corresponds to the current selection; unfortunately needed by the label provider
    private MenuManager contextMenuManager = new MenuManager("#PopupMenu");

    // hashmap to save/restore view's state when switching across editors
    private WeakHashMap<IEditorInput, ISelection> selectedElements = new WeakHashMap<IEditorInput, ISelection>();

    public ModuleParametersView() {
    }

    @Override
    public Control createViewControl(Composite parent) {
        Composite container = new Composite(parent, SWT.NONE);
        container.setLayout(new FillLayout());

        // add table viewer and set it as selection provider
        createTableViewer(container);
        getViewSite().setSelectionProvider(tableViewer);

        // add actions
        createActions();

        return container;
    }

    private void createTableViewer(Composite container) {
        // create table with columns
        Table table = new Table(container, SWT.MULTI | SWT.FULL_SELECTION | SWT.VIRTUAL);
        table.setHeaderVisible(true);
        table.setLinesVisible(true);
        parameterColumn = addTableColumn(table, "Parameter", 300);
        valueColumn = addTableColumn(table, "Value", 100);
        remarkColumn = addTableColumn(table, "Remark", 300);

        // add a TableViewer on top
        tableViewer = new TableViewer(table);
        tableViewer.setLabelProvider(new TableLabelProvider() {
            @Override
            public String getColumnText(Object element, int columnIndex) {
                Assert.isTrue(columnIndex<=2);
                if (element instanceof ParamResolution) {
                    ParamResolution res = (ParamResolution) element;
                    switch (columnIndex) {
                        case 0: return res.fullPath + "." + res.paramDeclaration.getName();
                        case 1: return InifileUtils.getParamValue(res, inifileDocument);
                        case 2: return InifileUtils.getParamRemark(res, inifileDocument);
                    }
                }
                return columnIndex==0 ? element.toString() : "";
            }

            @Override
            public Image getColumnImage(Object element, int columnIndex) {
                if (columnIndex!=0)
                    return null;
                if (element instanceof ParamResolution)
                    return InifileUtils.suggestImage(((ParamResolution) element).type);
                else
                    return null;
            }

        });
        tableViewer.setContentProvider(new ArrayContentProvider());

        // add tooltip support to the table
        new HoverSupport().adapt(tableViewer.getTable(), new IHoverInfoProvider() {
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                Item item = tableViewer.getTable().getItem(new Point(x,y));
                Object element = item==null ? null : item.getData();
                if (element instanceof ParamResolution) {
                    ParamResolution res = (ParamResolution) element;
                    if (res.section != null && res.key != null)
                        //XXX make sure "res" and inifile editor refer to the same IFile!!!
                        return new HtmlHoverInfo(InifileHoverUtils.getEntryHoverText(res.section, res.key, inifileDocument, inifileAnalyzer));
                    else
                        return new HtmlHoverInfo(InifileHoverUtils.getParamHoverText(res.elementPath, res.paramDeclaration, res.paramAssignment));
                }
                return null;
            }
        });

        // things to do when table selection changes
        tableViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                IEditorPart editor = getAssociatedEditor();
                if (!event.getSelection().isEmpty() && editor!=null) {
                    // remember selection (we'll try to restore it after table rebuild)
                    selectedElements.put(editor.getEditorInput(), event.getSelection());
                    // try to highlight the given element in the inifile editor
                    SectionKey sel = getSectionKeyFromSelection();
                    //XXX make sure "res" and inifile editor refer to the same IFile!!!
                    if (sel != null && editor instanceof IGotoInifile && editor != getActivePart())
                        ((IGotoInifile)editor).gotoEntry(sel.section, sel.key, IGotoInifile.Mode.AUTO);
                }
            }
        });

        // create context menu
        getViewSite().registerContextMenu(contextMenuManager, tableViewer);
        tableViewer.getTable().setMenu(contextMenuManager.createContextMenu(tableViewer.getTable()));

    }

    private TableColumn addTableColumn(final Table table, String text, int width) {
        TableColumn column = new TableColumn(table, SWT.NONE);
        column.setWidth(width);
        column.setText(text);
        column.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                TableColumn tableColumn = (TableColumn)e.widget;
                int sortDirection = (table.getSortColumn() == tableColumn && table.getSortDirection() == SWT.UP ? SWT.DOWN : SWT.UP);
                table.setSortColumn(tableColumn);
                table.setSortDirection(sortDirection);
                sortTableInput(tableColumn, sortDirection);
            }
        });
        return column;
    }

    private void createActions() {
        IAction pinAction = getOrCreatePinAction();

        Action toggleModeAction = new ActionExt("Show unassigned only", IAction.AS_CHECK_BOX,
                InifileEditorPlugin.getImageDescriptor("icons/full/elcl16/filter_ps.gif")) {
            @Override
            public void run() {
                unassignedOnly = !unassignedOnly;
                rebuildContent();
            }
        };

        final ActionExt gotoInifileAction = new ActionExt("Show in Ini File") {
            @Override
            public void run() {
                SectionKey sel = getSectionKeyFromSelection();
                IEditorPart associatedEditor = getAssociatedEditor();
                if (sel!=null && associatedEditor instanceof IGotoInifile) {
                    activateEditor(associatedEditor);
                    ((IGotoInifile)associatedEditor).gotoEntry(sel.section, sel.key, IGotoInifile.Mode.AUTO);
                }
            }
            public void selectionChanged(SelectionChangedEvent event) {
                SectionKey sel = getSectionKeyFromSelection();
                setEnabled(sel!=null);
            }
        };

        class GotoNedFileAction extends ActionExt {
            boolean gotoDecl;
            GotoNedFileAction(String text, ImageDescriptor image, boolean gotoDecl) {
                super(text, image);
                this.gotoDecl = gotoDecl;
            }
            @Override
            public void run() {
                INedElement sel = getNedElementFromSelection();
                if (sel!=null)
                    NedResourcesPlugin.openNedElementInEditor(sel);
            }
            public void selectionChanged(SelectionChangedEvent event) {
                INedElement sel = getNedElementFromSelection();
                setEnabled(sel!=null);
            }
            private INedElement getNedElementFromSelection() {
                Object element = ((IStructuredSelection)tableViewer.getSelection()).getFirstElement();
                if (element instanceof ParamResolution) {
                    ParamResolution res = (ParamResolution) element;
                    //return gotoDecl ? res.paramDeclNode : res.paramValueNode;
                    // experimental: disable "Open NED declaration" if it's the same as "Open NED value"
                    //return gotoDecl ? (res.paramDeclNode==res.paramValueNode ? null : res.paramDeclNode) : res.paramValueNode;
                    // experimental: disable "Open NED Value" if it's the same as the declaration
                    return gotoDecl ? res.paramDeclaration : (res.paramDeclaration == res.paramAssignment ? null : res.paramAssignment);
                }
                return null;
            }
        };
        ActionExt gotoNedDeclarationAction = new GotoNedFileAction("Open NED Declaration", null, true);
        ActionExt gotoNedValueAction = new GotoNedFileAction("Open NED Value", null, false);

        final ActionExt copyParametersAction = new ActionExt("Copy") {
            public void run() {
                List<ParamResolution> selection = getSelectedParamResolutions();
                if (!selection.isEmpty()) {
                    String text = "";
                    for (ParamResolution param : selection) {
                        String key = param.fullPath + "." + param.paramDeclaration.getName();
                        String value = StringUtils.defaultString(InifileUtils.getParamValue(param, inifileDocument));
                        text += key + " = " + value + "\n";
                    }

                    Clipboard clipboard = new Clipboard(Display.getDefault());
                    TextTransfer textTransfer = TextTransfer.getInstance();
                    clipboard.setContents(new String[] {text}, new Transfer[] {textTransfer});
                    clipboard.dispose();
                }
            }

            public void selectionChanged(SelectionChangedEvent event) {
                setEnabled(inifileDocument != null && !getSelectedParamResolutions().isEmpty());
            }

            private List<ParamResolution> getSelectedParamResolutions() {
                IStructuredSelection selection = (IStructuredSelection)tableViewer.getSelection();
                List<ParamResolution> paramResolutions = new ArrayList<ParamResolution>();
                for (Object object : selection.toList())
                    if (object instanceof ParamResolution)
                        paramResolutions.add((ParamResolution)object);
                return paramResolutions;
            }
        };

        tableViewer.addSelectionChangedListener(gotoInifileAction);
        tableViewer.addSelectionChangedListener(gotoNedValueAction);
        tableViewer.addSelectionChangedListener(gotoNedDeclarationAction);
        tableViewer.addSelectionChangedListener(copyParametersAction);

        // add double-click support to the table
        tableViewer.addDoubleClickListener(new IDoubleClickListener() {
            public void doubleClick(DoubleClickEvent event) {
                gotoInifileAction.run();
            }
        });

        // build menus and toolbar
        contextMenuManager.add(gotoInifileAction);
        contextMenuManager.add(gotoNedValueAction);
        contextMenuManager.add(gotoNedDeclarationAction);
        contextMenuManager.add(copyParametersAction);
        contextMenuManager.add(new Separator());
        contextMenuManager.add(toggleModeAction);
        contextMenuManager.add(pinAction);

        IToolBarManager toolBarManager = getViewSite().getActionBars().getToolBarManager();
        toolBarManager.add(toggleModeAction);
        toolBarManager.add(pinAction);

        IMenuManager menuManager = getViewSite().getActionBars().getMenuManager();
        menuManager.add(toggleModeAction);
        menuManager.add(pinAction);
    }

    protected void sortTableInput(TableColumn column, final int sortDirection) {
        Object[] input = (Object[]) tableViewer.getInput();

        if (input != null) {
            int columnNumber = -1;
            if (column == parameterColumn)
                columnNumber = 0;
            else if (column == valueColumn)
                columnNumber = 1;
            else if (column == remarkColumn)
                columnNumber = 2;

            // sort using the label provider
            //XXX too slow! cache labels  before sorting?
            final int finalColumnNumber = columnNumber;
            final ITableLabelProvider labelProvider = (ITableLabelProvider) tableViewer.getLabelProvider();
            Arrays.sort(input, new Comparator<Object>() {
                public int compare(Object o1, Object o2) {
                    String label1 = labelProvider.getColumnText(o1, finalColumnNumber);
                    String label2 = labelProvider.getColumnText(o2, finalColumnNumber);
                    if (label1 == null) label1 = "";
                    if (label2 == null) label2 = "";
                    return (sortDirection == SWT.DOWN ? -1 : 1) * StringUtils.dictionaryCompare(label1, label2);
                }
            });

            tableViewer.refresh();
        }
    }

    protected SectionKey getSectionKeyFromSelection() {
        Object element = ((IStructuredSelection)tableViewer.getSelection()).getFirstElement();
        if (element instanceof ParamResolution) {
            ParamResolution res = (ParamResolution) element;
            if (res.section!=null && res.key!=null)
                return new SectionKey(res.section, res.key);
        }
        return null;
    }

    @Override
    protected void showMessage(String text) {
        inifileAnalyzer = null;
        inifileDocument = null;
        tableViewer.setInput(new Object[0]);
        super.showMessage(text);
    }

    @Override
    public void setFocus() {
        if (isShowingMessage())
            super.setFocus();
        else
            tableViewer.getTable().setFocus();
    }

    @Override
    public void buildContent(INedElement element, final InifileAnalyzer analyzer, String section, String key) {

        INedElement elementWithParameters = findAncestorWithParameters(element);
        if (elementWithParameters == null) {
            showMessage("No element with parameters selected.");
            return;
        }

        String text = null;

        //XXX why not unconditionally store analyzer and doc references???
        if (analyzer==null) {
            List<ParamResolution> pars = null;

            INedTypeResolver nedResolver = elementWithParameters.getContainingNedFileElement().getResolver();

            if (elementWithParameters instanceof SubmoduleElementEx) {
                SubmoduleElementEx submodule = (SubmoduleElementEx)elementWithParameters;
                INedTypeInfo typeInfo = submodule.getNedTypeInfo();
                text = "Submodule " + submodule.getName() + ": " + (typeInfo==null ? "(invalid submodule type)" : typeInfo.getName());
                pars = ParamCollector.collectParameters(submodule, nedResolver);
            }
            else if (elementWithParameters instanceof ConnectionElementEx) {
                ConnectionElementEx connection = (ConnectionElementEx)elementWithParameters;
                INedTypeInfo typeInfo = connection.getNedTypeInfo();
                text = "Connection " + connection.getSrcGateFullyQualified() + ": " + (typeInfo==null ? "(invalid channel type)" : typeInfo.getName());
                pars = typeInfo!=null ? ParamCollector.collectParameters(typeInfo, nedResolver) : new ArrayList<ParamResolution>();
            }
            else if (elementWithParameters instanceof INedTypeElement) {
                INedTypeInfo typeInfo = ((INedTypeElement)elementWithParameters).getNedTypeInfo();
                text = StringUtils.capitalize(elementWithParameters.getReadableTagName()) + ": " + typeInfo.getName();
                pars = ParamCollector.collectParameters(typeInfo, nedResolver);
            }
            else {
                showMessage("NED element should be a module type, a submodule or a connection.");
                return;
            }

            tableViewer.setInput(pars.toArray(new ParamResolution[0]));
        }
        else {
            if (section==null)
                section = GENERAL;
            hideMessage();

            try {
                // update table contents
                inifileAnalyzer = analyzer;
                inifileDocument = analyzer.getDocument();
                ITimeout timeout = new Timeout(50); // do not block the UI -- we'll be called back when analysis is ready anyway
                ParamResolution[] pars = unassignedOnly ? analyzer.getUnassignedParams(section, timeout) : analyzer.getParamResolutions(section, timeout);
                tableViewer.setInput(pars);

                // update label
                text = "Section ["+section+"]";
            } catch (ParamResolutionDisabledException e) {
                tableViewer.setInput(new ParamResolution[0]);
                setContentDescription("Turn on ini file analysis to display the parameters.");
                return;
            } catch (ParamResolutionTimeoutException e) {
                // Note: we continue to display the old data until new data become available, for smoother user experience.
                // However, this is not without dangers: e.g. if user clicks on a parameter which has since been deleted
                // from the NED file, it will likely result in an NPE...
                // TODO resolve the above issue somehow...
                setContentDescription("Waiting for ini file analysis to complete, currently displayed data may be out of date.");

                // add one-time listener to update the view once the data become available
                analyzer.addAnalysisListener(new IAnalysisListener() {
                    public void analysisCompleted(InifileAnalyzer analyzer) {
                        analyzer.removeAnalysisListener(this);
                        if (!ModuleParametersView.this.isDisposed())
                            scheduleRebuildContent();
                    }
                });
                return;
            }
        }

        // try to preserve selection
        ISelection oldSelection = selectedElements.get(getAssociatedEditor().getEditorInput());
        if (oldSelection != null)
            tableViewer.setSelection(oldSelection, true);

        // set label
        if (getPinnedToEditor() != null)
            text += " in " + getPinnedToEditor().getEditorInput().getName() + " (pinned)";
        text += ", " + (unassignedOnly ? "unassigned parameters" : "all parameters");
        setContentDescription(text);

        // set up filter
        if (unassignedOnly)
            tableViewer.setFilters(new ViewerFilter[] {new ViewerFilter() {
                @Override
                public boolean select(Viewer viewer, Object parentElement, Object element) {
                    return ((ParamResolution)element).type == ParamResolutionType.UNASSIGNED;
                }
            }});
        else
            tableViewer.resetFilters();
    }
}
