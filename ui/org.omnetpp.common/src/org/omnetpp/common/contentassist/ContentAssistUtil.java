package org.omnetpp.common.contentassist;

import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.jface.viewers.ColumnViewer;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.ui.TableTextCellEditor;

/**
 * Set up content assist on the given column of a table viewer.
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
	public static ContentAssistCommandAdapter configureTableColumnContentAssist(final ColumnViewer tableViewer, 
			int columnIndex, IContentProposalProvider valueProposalProvider, char[] activationChars, boolean replace) {

		// officially, it should be just this:
		final TableTextCellEditor cellEditor = (TableTextCellEditor) tableViewer.getCellEditors()[columnIndex];
		ContentAssistCommandAdapter commandAdapter = new ContentAssistCommandAdapter(cellEditor.getText(), 
				new TextContentAdapter(), valueProposalProvider, 
				ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, activationChars, true);
		commandAdapter.setProposalAcceptanceStyle(replace ? ContentProposalAdapter.PROPOSAL_REPLACE : ContentProposalAdapter.PROPOSAL_INSERT);
		cellEditor.setContentAssistAdapter(commandAdapter);

		// Following workaround is not needed. Problem was solved by modifying TableTextCellEditor NOT to commit
		// on focusLost event, if the focus goes to the content assist popup. This works fairly well.
		//
		// WORKAROUND: after selecting a proposal with the mouse, the cell editor loses focus and closes,
		// before the proposal could be inserted -- which is bad. Luckily, the cell editor itself still 
		// contains the updated value. So, we read the correct value from the cell editor, commit it,
		// and re-activate the cell editor with the earlier contents and caret position.
		//		commandAdapter.addContentProposalListener(new IContentProposalListener() {
		//			public void proposalAccepted(IContentProposal proposal) {
		//				// we only need this trick if the cell editor is inactive (i.e. mouse selection)
		//				if (!tableViewer.isCellEditorActive()) {
		//					Text t = (Text) cellEditor.getText();
		//					if (t.isDisposed())
		//						return; // out of luck, there's nothing we can do
		//					String oldText = t.getText();
		//					int oldCaretPosition = t.getCaretPosition();
		//
		//					Object element = ((IStructuredSelection)tableViewer.getSelection()).getFirstElement();
		//					int columnIndex = ArrayUtils.indexOf(tableViewer.getCellEditors(), cellEditor);
		//					tableViewer.editElement(element, columnIndex);
		//					t.setText(oldText);
		//					t.setSelection(oldCaretPosition);
		//
		//					String property = (String) tableViewer.getColumnProperties()[columnIndex];
		//					try {
		//						tableViewer.getCellModifier().modify(element, property, oldText); // BTW, this may throw exceptions
		//					} catch (Exception e) {
		//						System.out.println("Eating exception: "+e.getMessage());
		//					}
		//				}
		//			}
		//		});
		return commandAdapter;
	}

}
