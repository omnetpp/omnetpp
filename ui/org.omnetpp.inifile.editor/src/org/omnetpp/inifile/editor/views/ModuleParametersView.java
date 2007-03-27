package org.omnetpp.inifile.editor.views;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.IEditorPart;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
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
public class ModuleParametersView extends AbstractModuleView {
	private TableViewer tableViewer;
	private boolean unassignedOnly = true;  //TODO IAction to flip it
	
	public ModuleParametersView() {
	}

	@Override
	public void createPartControl(Composite parent) {
		GridLayout layout = new GridLayout();
		layout.marginWidth = layout.marginHeight = 0;
		parent.setLayout(layout);
		
		// create table with columns
		Table table = new Table(parent, SWT.SINGLE | SWT.FULL_SELECTION);
		table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
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
		hookListeners();
		
		IAction toggleModeAction = createToggleModeAction();
		getViewSite().getActionBars().getToolBarManager().add(toggleModeAction);
	}

	protected IAction createToggleModeAction() {
		Action action = new Action("Toggle display mode", InifileEditorPlugin.getImageDescriptor("icons/unsetparameters.gif")) {
			@Override
			public void run() {
				unassignedOnly = !unassignedOnly;
				tableViewer.refresh();  //XXX not enough!
			}
		};
		return action;
	}

	private void addTableColumn(Table table, String text, int width) {
		TableColumn column = new TableColumn(table, SWT.NONE);
		column.setWidth(width);
		column.setText(text);
	}

	protected Control getPartControl() {
		return tableViewer.getTable();
	}

	@Override
	public void buildContent(String moduleFullPath, String moduleTypeName, IInifileDocument doc) {
		//XXX consider changing the return type of NEDResourcesPlugin.getNEDResources() to INEDTypeResolver
		INEDTypeResolver nedResources = NEDResourcesPlugin.getNEDResources();
		ParameterData[] pars = InifileUtils.collectParameters(moduleFullPath, moduleTypeName, nedResources, doc, unassignedOnly);
		tableViewer.setInput(pars);
	}
}
