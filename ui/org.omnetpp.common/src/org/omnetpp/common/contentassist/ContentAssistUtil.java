package org.omnetpp.common.contentassist;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalListener;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.ui.TableTextCellEditor;

/**
 * 
 * @author Andras
 */
public class ContentAssistUtil {
	/**
	 * Set up content assist on the given column of a table viewer.
	 * @param tableViewer
	 * @param columnIndex
	 * @param valueProposalProvider
	 * @param replace  whether proposal replaces editor content or gets inserted
	 */
	public static ContentAssistCommandAdapter configureTableColumnContentAssist(final TableViewer tableViewer, 
			int columnIndex, IContentProposalProvider valueProposalProvider, boolean replace) {

		// officially, it should be just this:
		final TableTextCellEditor cellEditor = (TableTextCellEditor) tableViewer.getCellEditors()[columnIndex];
		ContentAssistCommandAdapter commandAdapter = new ContentAssistCommandAdapter(cellEditor.getText(), 
				new TextContentAdapter(), valueProposalProvider, 
				ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, null, true);
		commandAdapter.setProposalAcceptanceStyle(replace ? ContentProposalAdapter.PROPOSAL_REPLACE : ContentProposalAdapter.PROPOSAL_INSERT);

		// but, nothing is perfect...
		
		// WORKAROUND: after selecting a proposal with the mouse, the cell editor loses focus and closes,
		// before the proposal could be inserted -- which is bad. Luckily, the cell editor itself still 
		// contains the updated value. So, we read the correct value from the cell editor, commit it,
		// and re-activate the cell editor with the earlier contents and caret position.
		// This also does no harm even when the proposal was accepted with the Enter key.
		commandAdapter.addContentProposalListener(new IContentProposalListener() {
			public void proposalAccepted(IContentProposal proposal) {
				Text t = (Text) cellEditor.getText();
				if (t.isDisposed())
					return; // out of luck, there's nothing we can do
				String oldText = t.getText();
				int oldCaretPosition = t.getCaretPosition();

				Object element = ((IStructuredSelection)tableViewer.getSelection()).getFirstElement();
				int columnIndex = ArrayUtils.indexOf(tableViewer.getCellEditors(), cellEditor);
				tableViewer.editElement(element, columnIndex);
				t.setText(oldText);
				t.setSelection(oldCaretPosition);
				
				String property = (String) tableViewer.getColumnProperties()[columnIndex];
				tableViewer.getCellModifier().modify(element, property, oldText); // BTW, this may throw exceptions
			}
		});
		return commandAdapter;
	}

}
