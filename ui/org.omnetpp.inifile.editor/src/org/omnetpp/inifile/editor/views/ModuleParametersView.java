package org.omnetpp.inifile.editor.views;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.Arrays;
import java.util.Comparator;
import java.util.WeakHashMap;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.inifile.editor.IGotoInifile;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.actions.ActionExt;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.SectionKey;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;


/**
 * Displays module parameters recursively for a module type.
 * @author Andras
 */
//XXX table sorting doesn't work
public class ModuleParametersView extends AbstractModuleView {
	private TableViewer tableViewer;
	private boolean unassignedOnly = true;
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
		Table table = new Table(container, SWT.SINGLE | SWT.FULL_SELECTION | SWT.VIRTUAL);
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
						case 0: return res.moduleFullPath+"."+res.paramDeclNode.getName();
						case 1: return InifileAnalyzer.getParamValue(res, inifileDocument);
						case 2: return InifileAnalyzer.getParamRemark(res, inifileDocument);
					}
				}
				return columnIndex==0 ? element.toString() : "";
			}

			@Override
			public Image getColumnImage(Object element, int columnIndex) {
				if (columnIndex!=0)
					return null;
				if (element instanceof ParamResolution)
					return suggestImage(((ParamResolution) element).type);
				else
					return null;
			}

		});
		tableViewer.setContentProvider(new ArrayContentProvider());

 		// add tooltip support to the table
 		new HoverSupport().adapt(tableViewer.getTable(), new IHoverTextProvider() {
			public String getHoverTextFor(Control control, int x, int y, SizeConstraint outPreferredSize) {
				Item item = tableViewer.getTable().getItem(new Point(x,y));
				Object element = item==null ? null : item.getData();
				if (element instanceof ParamResolution) {
					ParamResolution res = (ParamResolution) element;
					if (res.section != null && res.key != null)
						//XXX make sure "res" and inifile editor refer to the same IFile!!!
						return InifileHoverUtils.getEntryHoverText(res.section, res.key, inifileDocument, inifileAnalyzer);
					else 
						return InifileHoverUtils.getParamHoverText(res.pathModules, res.paramDeclNode, res.paramValueNode);
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
					selectedElements.put(getAssociatedEditor().getEditorInput(), event.getSelection());

					// try to highlight the given element in the inifile editor
					SectionKey sel = getSectionKeyFromSelection();
					//XXX make sure "res" and inifile editor refer to the same IFile!!!
					if (sel != null && editor instanceof IGotoInifile)
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

		Action toggleModeAction = new ActionExt("Show all", IAction.AS_CHECK_BOX, 
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
				INEDElement sel = getNEDElementFromSelection();
				if (sel!=null)
					NEDResourcesPlugin.openNEDElementInEditor(sel);
			}
			public void selectionChanged(SelectionChangedEvent event) {
				INEDElement sel = getNEDElementFromSelection();
				setEnabled(sel!=null);
			}
			private INEDElement getNEDElementFromSelection() {
				Object element = ((IStructuredSelection)tableViewer.getSelection()).getFirstElement();
				if (element instanceof ParamResolution) {
					ParamResolution res = (ParamResolution) element;
					//return gotoDecl ? res.paramDeclNode : res.paramValueNode; 
					// experimental: disable "Open NED declaration" if it's the same as "Open NED value"
					//return gotoDecl ? (res.paramDeclNode==res.paramValueNode ? null : res.paramDeclNode) : res.paramValueNode;
					// experimental: disable "Open NED Value" if it's the same as the declaration
					return gotoDecl ? res.paramDeclNode : (res.paramDeclNode==res.paramValueNode ? null : res.paramValueNode);
				}
				return null;
			}
		};
		ActionExt gotoNedDeclarationAction = new GotoNedFileAction("Open NED Declaration", null, true); 
		ActionExt gotoNedValueAction = new GotoNedFileAction("Open NED Value", null, false); 
	
		tableViewer.addSelectionChangedListener(gotoInifileAction);
		tableViewer.addSelectionChangedListener(gotoNedValueAction);
		tableViewer.addSelectionChangedListener(gotoNedDeclarationAction);
	
		// add double-click support to the table
		tableViewer.getTable().addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetDefaultSelected(SelectionEvent e) {
				gotoInifileAction.run();
			}
		});
	
		// build menus and toolbar
		contextMenuManager.add(gotoInifileAction);
		contextMenuManager.add(gotoNedValueAction);
		contextMenuManager.add(gotoNedDeclarationAction);
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

	@SuppressWarnings("unchecked")
	protected void sortTableInput(TableColumn column, int sortDirection) {
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
			Arrays.sort(input, new Comparator() {
				public int compare(Object o1, Object o2) {
					String label1 = labelProvider.getColumnText(o1, finalColumnNumber);
					String label2 = labelProvider.getColumnText(o2, finalColumnNumber);
					if (label1 == null) label1 = "";
					if (label2 == null) label2 = "";
					return label1.compareToIgnoreCase(label2);
				}
			});

			// for decreasing order, reverse the array
			if (sortDirection==SWT.DOWN)
				ArrayUtils.reverse(input);

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
	public void buildContent(INEDElement module, InifileAnalyzer analyzer, String section, String key) {
		System.out.println("view: buildContent()");
		//XXX factor out common part of the two "if" branches
		//XXX why not unconditionally store analyzer and doc references???
		if (analyzer==null) {
			ParamResolution[] pars = null;
			if (module instanceof SubmoduleNode)
				pars = InifileAnalyzer.collectParameters((SubmoduleNode)module).toArray(new ParamResolution[]{});
			else if (module instanceof INedTypeNode)
				pars = InifileAnalyzer.collectParameters(((INedTypeNode)module).getNEDTypeInfo()).toArray(new ParamResolution[]{});
			if (pars == null) {
				showMessage("NED element should be a module type or a submodule.");
				return;
			}

			tableViewer.setInput(pars);

			// try to preserve selection
			ISelection oldSelection = selectedElements.get(getAssociatedEditor().getEditorInput());
			if (oldSelection != null)
				tableViewer.setSelection(oldSelection, true);
			
			//XXX set label
		}
		else {
			if (section==null)
				section = GENERAL;
			hideMessage();
			
			// update table contents
			inifileAnalyzer = analyzer;
			inifileDocument = analyzer.getDocument();
			ParamResolution[] pars = unassignedOnly ? analyzer.getUnassignedParams(section) : analyzer.getParamResolutions(section);
			tableViewer.setInput(pars);

			// try to preserve selection
			ISelection oldSelection = selectedElements.get(getAssociatedEditor().getEditorInput());
			if (oldSelection != null)
				tableViewer.setSelection(oldSelection, true);

			// update label
			String text = "Section ["+section+"]"; 
			if (getPinnedToEditor() != null)
				text += " in " + getPinnedToEditor().getEditorInput().getName() + " (pinned)"; 
			text += ", " + (unassignedOnly ? "unassigned parameters" : "all parameters");
			setContentDescription(text);
		}
	}

}
