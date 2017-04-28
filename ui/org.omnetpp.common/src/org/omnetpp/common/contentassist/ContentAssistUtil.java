/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.contentassist;

import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.FieldDecoration;
import org.eclipse.jface.fieldassist.FieldDecorationRegistry;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalListener;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.jface.viewers.ColumnViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.ui.TableTextCellEditor;

/**
 * Utility functions for setting up content assist on controls.
 * 
 * @author Andras
 */
public class ContentAssistUtil {
    /**
     * Setup content assist on a Text. 
     * 
     * IMPORTANT: This one assumes that proposals are instances of ContentProposal, 
     * our IContentProposal implementation. 
     */
    public static void configureText(Text text, IContentProposalProvider proposalProvider) {
        FieldDecoration contentAssistDecoration = FieldDecorationRegistry.getDefault().getFieldDecoration(FieldDecorationRegistry.DEC_CONTENT_PROPOSAL);
        ControlDecoration decorator = new ControlDecoration(text, SWT.TOP | SWT.LEFT, text.getParent());
        decorator.setImage(contentAssistDecoration.getImage());
        decorator.setDescriptionText(contentAssistDecoration.getDescription());
        ContentAssistCommandAdapter commandAdapter = new ContentAssistCommandAdapter(text,
                new TextContentAdapter2(),
                proposalProvider,
                ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS,   /*commandId. "null" works equally well. XXX no binding is found for the default command "org.eclipse.ui.edit.text.contentAssist.proposals", that's why it says "null" in the bubble. how to fix it? */
                "( ".toCharArray() /*auto-activation*/);


        final IControlContentAdapterEx contentAdapter = (IControlContentAdapterEx)commandAdapter.getControlContentAdapter();

        commandAdapter.setProposalAcceptanceStyle(ContentProposalAdapter.PROPOSAL_IGNORE);
        commandAdapter.addContentProposalListener(new IContentProposalListener() {
            public void proposalAccepted(IContentProposal proposal) {
                ContentProposal filterProposal = (ContentProposal)proposal;
                contentAdapter.replaceControlContents(
                        text,
                        filterProposal.getStartIndex(),
                        filterProposal.getEndIndex(),
                        filterProposal.getContent(),
                        filterProposal.getCursorPosition());
            }
        });
    }
    
    
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
        //      commandAdapter.addContentProposalListener(new IContentProposalListener() {
        //          public void proposalAccepted(IContentProposal proposal) {
        //              // we only need this trick if the cell editor is inactive (i.e. mouse selection)
        //              if (!tableViewer.isCellEditorActive()) {
        //                  Text t = (Text) cellEditor.getText();
        //                  if (t.isDisposed())
        //                      return; // out of luck, there's nothing we can do
        //                  String oldText = t.getText();
        //                  int oldCaretPosition = t.getCaretPosition();
        //
        //                  Object element = ((IStructuredSelection)tableViewer.getSelection()).getFirstElement();
        //                  int columnIndex = ArrayUtils.indexOf(tableViewer.getCellEditors(), cellEditor);
        //                  tableViewer.editElement(element, columnIndex);
        //                  t.setText(oldText);
        //                  t.setSelection(oldCaretPosition);
        //
        //                  String property = (String) tableViewer.getColumnProperties()[columnIndex];
        //                  try {
        //                      tableViewer.getCellModifier().modify(element, property, oldText); // BTW, this may throw exceptions
        //                  } catch (Exception e) {
        //                      Debug.println("Eating exception: "+e.getMessage());
        //                  }
        //              }
        //          }
        //      });
        return commandAdapter;
    }

}
