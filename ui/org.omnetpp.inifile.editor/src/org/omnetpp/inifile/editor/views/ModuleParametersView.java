package org.omnetpp.inifile.editor.views;

import java.util.Arrays;
import java.util.Comparator;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.ParamResolution;

/**
 * Displays module parameters recursively for module type.
 * @author Andras
 */
//XXX add ordering support to the table
//XXX follow selection
//XXX context menu with "Go to NED file" and "Go to ini file"
//XXX fix hardcoded "Parameters" section
public class ModuleParametersView extends AbstractModuleView {
	private TableViewer tableViewer;
	private boolean unassignedOnly = true;
	private TableColumn parameterColumn;
	private TableColumn valueColumn;
	private TableColumn remarkColumn;
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
