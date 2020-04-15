package freemarker.eclipse.editors;

import java.util.ResourceBundle;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.Position;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.ui.IEditorActionBarContributor;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.part.EditorActionBarContributor;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.eclipse.ui.texteditor.MarkerAnnotation;
import org.eclipse.ui.texteditor.TextOperationAction;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;

import freemarker.eclipse.outline.OutlinePage;

/**
 * @version $Id: FreemarkerEditor.java,v 1.3 2004/02/05 00:16:23 stephanmueller Exp $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 */
public class FreemarkerEditor extends TextEditor {

    private ITokenManager tokenManager;
    private OutlinePage fOutlinePage;
    private ReconcilingStrategy fReconcilingStrategy;
    private int fLastCursorLine = -1;


    public FreemarkerEditor() {
        super();
        tokenManager = new TokenManager();
        fReconcilingStrategy = new ReconcilingStrategy(this);
        setSourceViewerConfiguration(new Configuration(this, tokenManager));
    }

    public void dispose() {
        tokenManager.dispose();
        super.dispose();
    }

    @SuppressWarnings({"rawtypes", "unchecked"})
    public Object getAdapter(Class aClass) {
        Object adapter;
        if (aClass.equals(IContentOutlinePage.class)) {
            if (fOutlinePage == null) {
                fOutlinePage = new OutlinePage(this);
                if (getEditorInput() != null) {
                    fOutlinePage.setInput(getEditorInput());
                }
            }
            adapter = fOutlinePage;
        } else {
            adapter = super.getAdapter(aClass);
        }
        return adapter;
    }

    protected void handleCursorPositionChanged() {
        super.handleCursorPositionChanged();
        int line = getCursorLine();
        if (line > 0 && line != fLastCursorLine) {
            fLastCursorLine = line;
            if (fOutlinePage != null) {

            }
        }
    }

    protected void handleEditorInputChanged() {

    }

    public IDocument getDocument() {
        return getDocumentProvider().getDocument(getEditorInput());
    }

    @Override
    protected void createActions() {
        super.createActions();

        ResourceBundle bundle = EditorMessages.getResourceBundle();
        IAction a = new TextOperationAction(bundle, "ContentAssistProposal.", this, ISourceViewer.CONTENTASSIST_PROPOSALS);
        a.setActionDefinitionId(ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS);
        setAction(ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, a);
    }

    public int getCursorLine() {
        int line = -1;

        ISourceViewer sourceViewer = getSourceViewer();
        if (sourceViewer != null) {
            int caret = sourceViewer.getVisibleRegion().getOffset() +
                                 sourceViewer.getTextWidget().getCaretOffset();
            IDocument document = sourceViewer.getDocument();
            if (document != null) {
                try {
                    line = document.getLineOfOffset(caret) + 1;
                } catch (BadLocationException e) {
                    e.printStackTrace();
                }
            }
        }
        return line;
    }

    public void selectTextRange(int beginLine, int beginColumn, int length) {
        int bOffset = 0;
        try {
            bOffset = getDocument().getLineInformation(beginLine-1).getOffset();
            setHighlightRange(bOffset+beginColumn-1, length , true);
        } catch (Exception e) {

        }
    }

    public void updateOutlinePage() {
        if (fOutlinePage != null) {
            fOutlinePage.update();
        }
    }

    public ReconcilingStrategy getReconcilingStrategy() {
        return fReconcilingStrategy;
    }


    public void displayErrorMessage(String aMessage) {
        IEditorActionBarContributor contributor =
                                     getEditorSite().getActionBarContributor();
        if (contributor != null &&
                          contributor instanceof EditorActionBarContributor) {
            IStatusLineManager manager = ((EditorActionBarContributor)
                           contributor).getActionBars().getStatusLineManager();
            manager.setErrorMessage(aMessage);

        }
    }

    public void addProblemMarker(String aMessage, int aLine) {
        IFile file = ((IFileEditorInput)getEditorInput()).getFile();
        try {
            IMarker marker = file.createMarker(IMarker.PROBLEM);
            marker.setAttribute(IMarker.SEVERITY, IMarker.SEVERITY_ERROR);
            marker.setAttribute(IMarker.LINE_NUMBER, aLine);
            marker.setAttribute(IMarker.MESSAGE, aMessage);
            Position pos = new Position(getDocument().getLineOffset(aLine - 1));
            getSourceViewer().getAnnotationModel().addAnnotation(
                                            new MarkerAnnotation(marker), pos);

        } catch (Exception e) {

        }
    }


}
