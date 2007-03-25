package org.omnetpp.inifile.editor.views;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.InifileUtils.ParameterData;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * Displays module parameters recursively for module type.
 * @author Andras
 */
public class UnassignedParametersView extends ViewPart {
	private TableViewer tableViewer;
	private ISelectionListener selectionChangedListener;
	private boolean unassignedOnly = true;  //TODO IAction to flip it
	
	public UnassignedParametersView() {
	}

	@Override
	public void createPartControl(Composite parent) {
		// create table with columns
		Table table = new Table(parent, SWT.SINGLE | SWT.FULL_SELECTION);
		table.setHeaderVisible(true);
		table.setLinesVisible(true);
		addTableColumn(table, "Parameter", 300);
		addTableColumn(table, "Value", 100);
		addTableColumn(table, "Remark", 300);
		
		// add a TableViewer on top
		tableViewer = new TableViewer(table);
		tableViewer.setLabelProvider(new TableLabelProvider() {
			@Override
			public String getColumnText(Object element, int columnIndex) {
				Assert.isTrue(columnIndex<=2);
				if (element instanceof ParameterData) {
					ParameterData par = (ParameterData) element;
					switch (columnIndex) {
					case 0: return par.moduleFullPath+"."+par.parameterName;
					case 1: return par.value;
					case 2: return par.remark;
					}
				}
				return columnIndex==0 ? element.toString() : "";
			}
			
		});
		tableViewer.setContentProvider(new ArrayContentProvider());
		hookSelectionChangedListener();
		
		getViewSite().getActionBars().getToolBarManager().add(new Action() {
			@Override
			public void run() {
				unassignedOnly = !unassignedOnly;
				tableViewer.refresh();
			}
		});
	}

	private void addTableColumn(Table table, String text, int width) {
		TableColumn column = new TableColumn(table, SWT.NONE);
		column.setWidth(width);
		column.setText(text);
	}

	@Override
	public void dispose() {
		unhookSelectionChangedListener();
		super.dispose();
	}
	
	private void hookSelectionChangedListener() {
		selectionChangedListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				setViewerInput(selection);
			}
		};
		getSite().getPage().addPostSelectionListener(selectionChangedListener);
	}
	
	private void unhookSelectionChangedListener() {
		if (selectionChangedListener != null)
			getSite().getPage().removePostSelectionListener(selectionChangedListener);
	}
	
	@Override
	public void setFocus() {
		tableViewer.getTable().setFocus();
	}

	public void setViewerInput(ISelection selection) {
		IEditorPart editor = getSite().getWorkbenchWindow().getActivePage().getActiveEditor();
		if (editor instanceof InifileEditor) {
			InifileEditor inifileEditor = (InifileEditor) editor;
			IInifileDocument doc = inifileEditor.getEditorData().getInifileDocument();

			//XXX consider changing the return type of NEDResourcesPlugin.getNEDResources() to INEDTypeResolver
			INEDTypeResolver nedResources = NEDResourcesPlugin.getNEDResources();

			String networkName = doc.getValue("General", "network");
			if (networkName == null) {
				displayMessage("Network not specified (no [General]/network= setting)");
				return;
			}

			ParameterData[] pars = InifileUtils.collectParameters(networkName, networkName, nedResources, doc, unassignedOnly);
			tableViewer.setInput(pars);
		}
		else {
			displayMessage(editor==null ? "No editor is open." : "Editor is not an inifile editor.");
		}
	}

	private void displayMessage(String text) {
		tableViewer.setInput(new String[] {text});
	}
}
