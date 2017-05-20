/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.contentassist;

import org.eclipse.jface.dialogs.PopupDialog;
import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.FieldDecoration;
import org.eclipse.jface.fieldassist.FieldDecorationRegistry;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalListener;
import org.eclipse.jface.fieldassist.IContentProposalListener2;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.jface.util.Geometry;
import org.eclipse.jface.viewers.ColumnViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ExtendedModifyEvent;
import org.eclipse.swt.custom.ExtendedModifyListener;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Monitor;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.ui.StyledTextUndoRedoManager;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.common.util.ReflectionUtils;

/**
 * Utility functions for setting up content assist on controls.
 *
 * @author Andras
 */
//TODO in an empty StyledText, content assist does not work properly. proposalAccepted() is not called, apparently because the popup somehow closes too early 
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

    public static void configureStyledText(StyledText styledText, IContentProposalProvider proposalProvider) {
        FieldDecoration contentAssistDecoration = FieldDecorationRegistry.getDefault().getFieldDecoration(FieldDecorationRegistry.DEC_CONTENT_PROPOSAL);
        ControlDecoration decorator = new ControlDecoration(styledText, SWT.TOP | SWT.LEFT, styledText.getParent());
        decorator.setImage(contentAssistDecoration.getImage());
        decorator.setDescriptionText(contentAssistDecoration.getDescription());
        ContentAssistCommandAdapter commandAdapter = new ContentAssistCommandAdapter(styledText,
                new StyledTextContentAdapter(),
                proposalProvider,
                ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS,   /*commandId. "null" works equally well. XXX no binding is found for the default command "org.eclipse.ui.edit.text.contentAssist.proposals", that's why it says "null" in the bubble. how to fix it? */
                "( ".toCharArray() /*auto-activation*/);

        commandAdapter.setPropagateKeys(false);
        commandAdapter.setProposalAcceptanceStyle(ContentProposalAdapter.PROPOSAL_IGNORE);

        commandAdapter.addContentProposalListener(new IContentProposalListener() {
            @Override
            public void proposalAccepted(IContentProposal proposal) {
                ContentProposal filterProposal = (ContentProposal)proposal;
                final IControlContentAdapterEx contentAdapter = (IControlContentAdapterEx)commandAdapter.getControlContentAdapter();
                contentAdapter.replaceControlContents(
                        styledText,
                        filterProposal.getStartIndex(),
                        filterProposal.getEndIndex(),
                        filterProposal.getContent(),
                        filterProposal.getCursorPosition());
            }
        });

        // When using proposalAcceptanceStyle == PROPOSAL_IGNORE, ContentProposalAdapter
        // places the popup alongside the bottom edge of the text widget, not near the cursor.
        // Workaround: move the popup to the right place from the proposalPopupOpened() callback.
        commandAdapter.addContentProposalListener(new IContentProposalListener2() {
            @Override
            public void proposalPopupOpened(ContentProposalAdapter adapter) {
                PopupDialog popup = (PopupDialog)ReflectionUtils.getFieldValue(adapter, "popup");  // Khmm... Cause there's no getPopup().
                adjustPopupBounds(popup, styledText, commandAdapter);
            }

            @Override
            public void proposalPopupClosed(ContentProposalAdapter adapter) {
            }
        });

        // When accepting the proposal with the Enter key, the text widget will also inserts
        // the newline, as if the content proposal popup wasn't there. It it because the text
        // widget owns the focus, not the popup.
        // Workaround: we detect the insertion of the newline from ExtendedModifyListener,
        // and remove it.
        styledText.addExtendedModifyListener(new ExtendedModifyListener() {
            @Override
            public void modifyText(ExtendedModifyEvent event) {
                if (commandAdapter.isProposalPopupOpen()) {
                    StyledText styledText = (StyledText)event.widget;
                    String insertedText = styledText.getTextRange(event.start,  event.length);
                    if (insertedText.equals("\n"))
                        StyledTextUndoRedoManager.getOrCreateManagerOf(styledText).undo();  // or: styledText.replaceTextRange(event.start, event.length, event.replacedText), but this pollutes the undo stack
                }
            }
        });
    }

    protected static void adjustPopupBounds(PopupDialog popup, Control control, ContentAssistCommandAdapter commandAdapter) {
        int POPUP_OFFSET = 3;
        int MAX_POPUP_WIDTH = 800; // practical upper limit
        Point controlLocation = control.getDisplay().map(control.getParent(), null, control.getLocation());
        Rectangle insertionBounds = commandAdapter.getControlContentAdapter().getInsertionBounds(control);
        int initialX = controlLocation.x + insertionBounds.x + POPUP_OFFSET;
        int initialY = controlLocation.y + insertionBounds.y + insertionBounds.height;
        Rectangle popupBounds = popup.getShell().getBounds();
        popupBounds.x = initialX;
        popupBounds.y = initialY;
        popupBounds.width = Math.min(popupBounds.width, MAX_POPUP_WIDTH);
        popupBounds = getConstrainedShellBounds(popupBounds);
        popup.getShell().setBounds(popupBounds);
    }

    // copied from org.eclipse.jface.window.Window
    protected static Rectangle getConstrainedShellBounds(Rectangle preferredSize) {
        Rectangle result = new Rectangle(preferredSize.x, preferredSize.y, preferredSize.width, preferredSize.height);

        Monitor mon = getClosestMonitor(Display.getCurrent(), Geometry.centerPoint(result));

        Rectangle bounds = mon.getClientArea();

        if (result.height > bounds.height)
            result.height = bounds.height;

        if (result.width > bounds.width)
            result.width = bounds.width;

        result.x = Math.max(bounds.x, Math.min(result.x, bounds.x + bounds.width - result.width));
        result.y = Math.max(bounds.y, Math.min(result.y, bounds.y + bounds.height - result.height));

        return result;
    }

    // copied from org.eclipse.jface.window.Window
    protected static Monitor getClosestMonitor(Display toSearch, Point toFind) {
        int closest = Integer.MAX_VALUE;

        Monitor[] monitors = toSearch.getMonitors();
        Monitor result = monitors[0];

        for (int idx = 0; idx < monitors.length; idx++) {
            Monitor current = monitors[idx];

            Rectangle clientArea = current.getClientArea();

            if (clientArea.contains(toFind))
                return current;

            int distance = Geometry.distanceSquared(Geometry.centerPoint(clientArea), toFind);
            if (distance < closest) {
                closest = distance;
                result = current;
            }
        }

        return result;
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
