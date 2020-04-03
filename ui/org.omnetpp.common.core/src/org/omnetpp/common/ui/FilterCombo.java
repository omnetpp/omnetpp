package org.omnetpp.common.ui;

import org.eclipse.jface.bindings.keys.KeyStroke;
import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.IControlContentAdapter;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Text;

/**
 * Custom combo-like compound widget built from a Text with content assist
 * plus a "down" button to explicitly toggle the opened state of the proposal popup.
 *
 * @author Andras
 */
public class FilterCombo extends Composite {
    private Text text;
    private Button button;
    private ContentProposalAdapterEx adapter;
    private boolean popupWasOpenWhenButtonWasClicked;

    // Class to make openProposalPopup() and closeProposalPopup() public
    class ContentProposalAdapterEx extends ContentProposalAdapter {
        public ContentProposalAdapterEx(Control control, IControlContentAdapter controlContentAdapter, IContentProposalProvider proposalProvider, KeyStroke keyStroke, char[] autoActivationCharacters) {
            super(control, controlContentAdapter, proposalProvider, keyStroke, autoActivationCharacters);
        }
        public void openProposalPopup() {
            super.openProposalPopup();
        }
        public void closeProposalPopup() {
            super.closeProposalPopup();
        }
    };

    public FilterCombo(Composite parent, int style) {
        super(parent, style);
        setLayout(new GridLayout(2, false));
        ((GridLayout)getLayout()).horizontalSpacing = 2;

        text = new Text(this, SWT.BORDER | SWT.SEARCH | SWT.ICON_CANCEL);
        text.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        button = new Button(this, SWT.ARROW | SWT.DOWN) {
            protected void checkSubclass() {}
            public Point computeSize(int wHint, int hHint, boolean changed) {
                Point size = super.computeSize(wHint,hHint,changed);
                size.y = 4; // pretend very small height, so that Text's default height takes effect
                return size;
            }
        };
        button.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false));

        setTabList(new Control[] { text });

        adapter = new ContentProposalAdapterEx(text, new TextContentAdapter(), null, null, null);
        adapter.setPropagateKeys(true);
        adapter.setProposalAcceptanceStyle(ContentProposalAdapter.PROPOSAL_REPLACE);

        // make accepting a proposal fire a defaultSelected event
        adapter.addContentProposalListener(p -> text.notifyListeners(SWT.DefaultSelection, new Event()));

        button.addMouseListener(MouseListener.mouseDownAdapter(c -> popupWasOpenWhenButtonWasClicked = adapter.isProposalPopupOpen()));

        button.addSelectionListener(SelectionListener.widgetSelectedAdapter((e)-> {
            // Note: We want the button to toggle the proposal popup. However,
            // adapter.isProposalPopupOpen() always returns false in this function,
            // because the popup was auto-closed due to losing its focus to the button widget.
            // The workaround is remember the popup's state in the mouse down event.
            // If it was open, the user's intention was to close it anyway, so we don't reopen it.
            text.setFocus();
            if (!popupWasOpenWhenButtonWasClicked)
                adapter.openProposalPopup();
        }));

        // let the Down arrow key and Ctrl+Space open the proposal popup
        text.addKeyListener(KeyListener.keyPressedAdapter(e -> {
            if (e.keyCode == SWT.ARROW_DOWN || (e.keyCode == SWT.SPACE && (e.stateMask & (SWT.CONTROL | SWT.COMMAND)) != 0))
                if (!adapter.isProposalPopupOpen())
                    adapter.openProposalPopup();
        }));
    }

    public void setContentproposalProvider(IContentProposalProvider proposalProvider) {
        adapter.setContentProposalProvider(proposalProvider);
    }

    public IContentProposalProvider getContentproposalProvider() {
        return adapter.getContentProposalProvider();
    }

    public void setMessage(String message) {
        text.setMessage(message);
    }

    public String setMessage() {
        return text.getMessage();
    }

    public void setText(String string) {
        text.setText(string);
    }

    public String getText() {
        return text.getText();
    }

    public void setToolTipText(String string) {
        text.setToolTipText(string);
    }

    public String getToolTipText() {
        return text.getToolTipText();
    }

    public void addSelectionListener(SelectionListener listener) {
        text.addSelectionListener(listener);
    }

    public void removeSelectionListener(SelectionListener listener) {
        text.removeSelectionListener(listener);
    }

    @Override
    public String toString() {
        return getClass().getSimpleName() + " {" + text.getMessage() + "}";
    }

}
