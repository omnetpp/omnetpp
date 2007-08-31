package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.tools.DirectEditManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ICellEditorValidator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.VerifyEvent;
import org.eclipse.swt.events.VerifyListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import org.omnetpp.figures.misc.IDirectEditSupport;
import org.omnetpp.ned.model.interfaces.IHasName;

/**
 * DirectEditManager for elements that can be renamed
 *
 * @author rhornig
 */
public class RenameDirectEditManager extends DirectEditManager {

    Font scaledFont;
    protected VerifyListener verifyListener;
    protected IDirectEditSupport directEditable;
    protected ICellEditorValidator validator;
    protected GraphicalEditPart sourcePart;

    /**
     * Creates a new RenameDirectEditManager with the given attributes.
     *
     * @param source
     *            the source EditPart
     * @param editorType
     *            type of editor
     * @param locator
     *            the CellEditorLocator
     */
    public RenameDirectEditManager(GraphicalEditPart source, Class<?> editorType,
            ICellEditorValidator validator,
            IDirectEditSupport directEditable) {
        super(source, editorType, directEditable.getDirectEditCellEditorLocator());
        this.directEditable = directEditable;
        this.sourcePart = source;
        this.validator = validator;
    }

    @Override
    protected void bringDown() {
        // This method might be re-entered when super.bringDown() is called.
        Font disposeFont = scaledFont;
        scaledFont = null;
        super.bringDown();
        if (disposeFont != null)
            disposeFont.dispose();
        directEditable.showLabelUnderCellEditor(true);
    }

    @Override
    protected void initCellEditor() {
        Text text = (Text) getCellEditor().getControl();
        verifyListener = new VerifyListener() {
            public void verifyText(VerifyEvent event) {
                Text text = (Text) getCellEditor().getControl();
                String oldText = text.getText();
                String leftText = oldText.substring(0, event.start);
                String rightText = oldText.substring(event.end, oldText.length());
                GC gc = new GC(text);
                Point size = gc.textExtent(leftText + event.text + rightText);
                gc.dispose();
                if (size.x != 0)
                    size = text.computeSize(size.x, SWT.DEFAULT);
                getCellEditor().getControl().setSize(size.x, size.y);
            }
        };
        text.addVerifyListener(verifyListener);

        // set the initial content from the model
        if (sourcePart.getModel() instanceof IHasName)
            getCellEditor().setValue(((IHasName)sourcePart.getModel()).getName());

        // hide the underlying label text
        directEditable.showLabelUnderCellEditor(false);
        // set the validator
        getCellEditor().setValidator(validator);
    }

    @Override
    protected void unhookListeners() {
        super.unhookListeners();
        Text text = (Text) getCellEditor().getControl();
        text.removeVerifyListener(verifyListener);
        verifyListener = null;
    }

    @Override
    protected void commit() {
        String message = getCellEditor().isValueValid() ? null : getCellEditor().getErrorMessage();
        if (message != null) {
            bringDown();
            MessageDialog.openError(PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell(),
                    "Rename failed", message);
        }
        else
            super.commit();
    }
}
