package org.omnetpp.inifile.editor.form;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalListener;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.inifile.editor.contentassist.InifileValueContentProposalProvider;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;

/**
 * Table based field editor for editing textual config entries
 * in all sections. Presents a table with a Section and a Value
 * columns, Value being editable.
 * 
 * @author Andras
 */
//XXX fix up content assist!!!
public class TextTableFieldEditor extends TableFieldEditor {

	public TextTableFieldEditor(Composite parent, ConfigurationEntry entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, entry, inifile, formPage, labelText);
	}

	protected TableViewer createTableViewer(Composite parent) {
		// add table
		Table table = new Table(parent, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION);
		table.setLayoutData(new GridData(320, table.getHeaderHeight()+4*table.getItemHeight()+2));
		table.setLinesVisible(true);
		table.setHeaderVisible(true);
		addTableColumn(table, "Section", 100);
		addTableColumn(table, "Value", 200);
		
		// set up tableViewer, label provider
		final TableViewer tableViewer = new TableViewer(table);
		tableViewer.setLabelProvider(new TableLabelProvider() {
			@Override
			public String getColumnText(Object element, int columnIndex) {
				String section = (String) element;
				switch (columnIndex) {
					case 0: return "["+section+"]";
					case 1: return getValueFromFile(section);
					default: throw new IllegalArgumentException();
				}
			}

			@Override
			public Image getColumnImage(Object element, int columnIndex) {
				if (columnIndex == 1) {
					String section = (String) element;
					IMarker[] markers = InifileUtils.getProblemMarkersFor(section, entry.getKey(), inifile);
					return getProblemImage(markers, true);
				}
				return null;
			}
		});
		
		// set up cell editor for value column
		tableViewer.setColumnProperties(new String[] {"section", "value"});
		final TableTextCellEditor[] cellEditors = new TableTextCellEditor[] {null, new TableTextCellEditor(tableViewer,1)};
		tableViewer.setCellEditors(cellEditors);
		tableViewer.setCellModifier(new ICellModifier() {
			public boolean canModify(Object element, String property) {
				return property.equals("value");
			}

			public Object getValue(Object element, String property) {
				Assert.isTrue(property.equals("value"));
				String section = (String) element;
				return getValueFromFile(section);
			}

			public void modify(Object element, String property, Object value) {
			    if (element instanceof Item)
			    	element = ((Item) element).getData(); // workaround, see super's comment
				Assert.isTrue(property.equals("value"));
				String section = (String) element;
				setValueInFile(section, (String) value);
				tableViewer.refresh();
			}
		});
		
		// content assist for the Value column
		IContentProposalProvider valueProposalProvider = new InifileValueContentProposalProvider(null, null, inifile, null) {
			@Override
			public IContentProposal[] getProposals(String contents, int position) {
				// we need to reconfigure the proposal provider on the fly to know about the current section
				String section = (String)( (IStructuredSelection)tableViewer.getSelection()).getFirstElement();
				setInifileEntry(section, entry.getKey()); // set context for proposal calculation
				return super.getProposals(contents, position);
			}
		};
		
		final ContentAssistCommandAdapter commandAdapter = new ContentAssistCommandAdapter(cellEditors[1].getText(), 
				new TextContentAdapter(), valueProposalProvider, 
				ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, null, true);

		//FIXME: our proposal provider drops the prefix! After "Fl"<Ctrl+Space>,<Enter>, result is "atnet"! 
		// The proposal provider should know whether it wants to "replace" or "insert" (?)
		// vagy tenyleg Tominak a TextContentAdapter2-jet kellene hasznalni..? (ha lehet, mert igy tunik a proposalAccepted() se mindig hivodik meg, pl Enter-re nem...)
		//commandAdapter.setProposalAcceptanceStyle(ContentProposalAdapter.PROPOSAL_REPLACE);

		// Note: after selecting a proposal with the mouse, the cell editor loses focus and closes,
		// before the proposal could be inserted. Luckily, the cell editor itself still contains
		// the updated value. So, as a workaround, we re-activate the cell editor, and restore
		// its previous contents. This does no harm even when the proposal was accepted with
		// the Enter key.
		//XXX do the same in the Parameters table
		commandAdapter.addContentProposalListener(new IContentProposalListener() {
			public void proposalAccepted(IContentProposal proposal) {
				Text t = cellEditors[1].getText();
				String oldText = t.getText();
				int oldCaretPosition = t.getCaretPosition();

				Object element = ((IStructuredSelection)tableViewer.getSelection()).getFirstElement();
				tableViewer.editElement(element, 1);
				t.setText(oldText);
				t.setSelection(oldCaretPosition);
			}
		});

		return tableViewer; 
	}
	
}
