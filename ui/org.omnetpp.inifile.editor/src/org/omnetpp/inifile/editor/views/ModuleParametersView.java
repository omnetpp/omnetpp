package org.omnetpp.inifile.editor.views;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.ParamResolution;

/**
 * Displays module parameters recursively for module type.
 * @author Andras
 */
//XXX add ordering support to the table
//XXX rewrite to make use of InifileAnalyzer
public class ModuleParametersView extends AbstractModuleView {
	private TableViewer tableViewer;
	private boolean unassignedOnly = true;
	private IInifileDocument inifileDocument; // corresponds to the current selection; unfortunately needed by the label provider
	
	/**
	 * Node contents for the GenericTreeNode tree that is displayed in the view
	 */
	//XXX needed?
	private static class ErrorNode {
		String text;
		ErrorNode(String text) {
			this.text = text;
		}
		@Override
		public String toString() {
			return text;
		}
		@Override
		public boolean equals(Object obj) {
			if (this == obj)
				return true;
			if (obj == null)
				return false;
			if (getClass() != obj.getClass())
				return false;
			final ErrorNode other = (ErrorNode) obj;
			if (text == null) {
				if (other.text != null)
					return false;
			} else if (!text.equals(other.text))
				return false;
			return true;
		}
	}

	public ModuleParametersView() {
	}

	@Override
	public Control createViewControl(Composite parent) {
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
				if (element instanceof ParamResolution) {
					ParamResolution par = (ParamResolution) element;
					switch (columnIndex) {
						case 0: return par.moduleFullPath+"."+par.paramNode.getName();
						case 1: return getValueAndRemark(par, inifileDocument)[0]; // value
						case 2: return getValueAndRemark(par, inifileDocument)[1]; // remark
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
				else if (element instanceof ErrorNode)
					return InifileEditorPlugin.getImage(ICON_ERROR);
				else
					return null;
			}
			
		});
		tableViewer.setContentProvider(new ArrayContentProvider());
		
		IAction toggleModeAction = createToggleModeAction();
		getViewSite().getActionBars().getToolBarManager().add(toggleModeAction);
		
		return table;
	}

	protected IAction createToggleModeAction() {
		Action action = new Action("Toggle display mode", InifileEditorPlugin.getImageDescriptor("icons/unsetparameters.gif")) {
			@Override
			public void run() {
				unassignedOnly = !unassignedOnly;
				rebuildContent();
			}
		};
		return action;
	}

	private void addTableColumn(Table table, String text, int width) {
		TableColumn column = new TableColumn(table, SWT.NONE);
		column.setWidth(width);
		column.setText(text);
	}

	@Override
	protected void displayMessage(String text) {
		super.displayMessage(text);
		inifileDocument = null;
		tableViewer.setInput(null);
	}
	
	@Override
	public void buildContent(String moduleFullPath, String moduleTypeName, InifileAnalyzer ana) {
		String section = "Parameters"; //XXX
		ParamResolution[] pars = unassignedOnly ? ana.getUnassignedParams(section) : ana.getParamResolutions(section); //XXX or maybe the resolutions for the selected key, etc
		tableViewer.setInput(pars);
	}
}
