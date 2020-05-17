/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.contentassist;

import org.eclipse.jface.dialogs.PopupDialog;
import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalListener;
import org.eclipse.jface.fieldassist.IContentProposalListener2;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.jface.util.Geometry;
import org.eclipse.jface.viewers.ColumnViewer;
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
 * Content Assist concepts 101 (what is what):
 *
 * - IContentProposalProvider is what gives the proposals, based on content.
 *
 * - ContentProposalAdapter is what orchestrates the UI: opens and closes the
 *   proposal popup, juggles keypresses between the popup and the text widget, etc.
 *   This is the tricky part, with lots of hacks.
 *
 * - ContentAssistCommandAdapter subclasses ContentProposalAdapter, and adds the feature
 *   of reacting to the platform standard Content Assist command instead of an explicitly
 *   specified keystroke.
 *
 * - IControlContentAdapter, i.e. TextContentAdapterEx and StyledTextContentAdapter, provide
 *   widget abstraction. They know how to get the cursor position, the content etc
 *   out of the control, and how to insert the accepted proposal back to it.
 *
 * @author Andras
 */
public class ContentAssistUtil {
    /**
     * Set up content assist on a (possibly multi-line) Text control.
     * IMPORTANT: This function assumes that proposals are instances of IContentProposalEx.
     */
    public static void configureText(Text text, IContentProposalProvider proposalProvider, String autoactivationChars) {
        configureTextOrStyledText(text, new TextContentAdapterEx(), proposalProvider, autoactivationChars);
    }

    /**
     * Set up content assist on a StyledText control. It currently doesn't work perfectly, see comment inside.
     * IMPORTANT: This function assumes that proposals are instances of IContentProposalEx.
     */
    public static void configureStyledText(StyledText styledText, IContentProposalProvider proposalProvider, String autoactivationChars) {

        // Issue: Under some circumstances, e.g. in an empty StyledText, accepting the proposal
        // with the Enter key doesn't work properly: it inserts a newline instead of the
        // proposal text. Accepting the proposal with the mouse is OK.
        //
        // Apparently it works wrong if the StyledText contents doesn't include any of the
        // autoactivation characters anywhere. If it contains an autoactivation character
        // (in our case: space,dot,paren,etc.) either before the cursor or after it,
        // content assist works just fine.
        //
        // Background: apparently proposalAccepted() is not called, apparently because the popup
        // closes too early. The culprit seem to be ContentProposalAdapter.shouldPopupRemainOpen()
        // which checks exactly that (whether the control's content contains any autoactivation
        // characters, and returns true/false based on that), and is used in an event listener as:
        //
        //     if (!shouldPopupRemainOpen())
        //         closeProposalPopup();
        //
        // The implementation of ContentProposalAdapter.shouldPopupRemainOpen() is probably misguided, but oh well.
        //
        // Workaround: we add "\n" to the autoactivation characters. This seems to completely solve
        // the problem (strangely even in the single-line content case, probably due to another bug
        // of superfluous "\n" insertion, for which the workaround is the ModifyListener below.)
        //

        String NEWLINE_AS_WORKAROUND = "\n"; // don't remove -- it fixes a bug that prevents accepted proposals from being inserted into a nearly-empty StyledText widget, see above comment

        ContentProposalAdapter adapter = configureTextOrStyledText(styledText, new StyledTextContentAdapter(), proposalProvider, autoactivationChars + NEWLINE_AS_WORKAROUND);

        // When accepting the proposal with the Enter key, the text widget will also inserts
        // the newline, as if the content proposal popup wasn't there. It it because the text
        // widget owns the focus, not the popup.
        // Workaround: we detect the insertion of the newline from ExtendedModifyListener,
        // and remove it.
        styledText.addExtendedModifyListener(new ExtendedModifyListener() {
            @Override
            public void modifyText(ExtendedModifyEvent event) {
                if (adapter.isProposalPopupOpen()) {
                    String insertedText = styledText.getTextRange(event.start,  event.length);
                    if (insertedText.equals(NEWLINE_AS_WORKAROUND))
                        StyledTextUndoRedoManager.getOrCreateManagerOf(styledText).undo();  // or: styledText.replaceTextRange(event.start, event.length, event.replacedText), but that pollutes the undo stack
                }
            }
        });
    }

    public static ContentProposalAdapter configureTextOrStyledText(Control text, IControlContentAdapterEx contentAdapter, IContentProposalProvider proposalProvider, String autoactivationChars) {
        // note: if we wanted to keep things simple, we could use the plain ContentProposalAdapter, as its subclass ContentAssistCommandAdapter only adds the command support
        ContentProposalAdapter adapter = new ContentAssistCommandAdapter(text,
                contentAdapter,
                proposalProvider,
                null /* use the default Content Assist command */,
                autoactivationChars.toCharArray(),
                true /* install decoration */);

        adapter.setProposalAcceptanceStyle(ContentProposalAdapter.PROPOSAL_IGNORE);
        //adapter.setPropagateKeys(false);

        adapter.addContentProposalListener(new IContentProposalListener() {
            public void proposalAccepted(IContentProposal proposal) {
                IContentProposalEx filterProposal = (IContentProposalEx)proposal;
                contentAdapter.replaceControlContents(
                        text,
                        filterProposal.getStartIndex(),
                        filterProposal.getEndIndex(),
                        filterProposal.getContent(),
                        filterProposal.getCursorPosition());
            }
        });

        // When using proposalAcceptanceStyle == PROPOSAL_IGNORE, ContentProposalAdapter
        // places the popup alongside the bottom edge of the text widget, not near the cursor.
        // Workaround: move the popup to the right place from the proposalPopupOpened() callback.
        adapter.addContentProposalListener(new IContentProposalListener2() {
            @Override
            public void proposalPopupOpened(ContentProposalAdapter adapter) {
                PopupDialog popup = (PopupDialog)ReflectionUtils.getFieldValue(adapter, "popup");  // Khmm... Cause there's no getPopup().
                adjustPopupBounds(popup, text, adapter);
            }

            @Override
            public void proposalPopupClosed(ContentProposalAdapter adapter) {
            }
        });

        return adapter;
    }

    protected static void adjustPopupBounds(PopupDialog popup, Control control, ContentProposalAdapter adapter) {
        int POPUP_OFFSET = 3;
        int MAX_POPUP_WIDTH = 800; // practical upper limit
        Point controlLocation = control.getDisplay().map(control.getParent(), null, control.getLocation());
        Rectangle insertionBounds = adapter.getControlContentAdapter().getInsertionBounds(control);
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
