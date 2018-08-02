package org.omnetpp.scave.editors;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.operation.IRunnableContext;
import org.eclipse.jface.text.Document;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.source.IAnnotationModel;
import org.eclipse.jface.text.source.projection.ProjectionAnnotationModel;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.texteditor.AbstractDocumentProvider;
import org.omnetpp.common.ui.TimeTriggeredProgressMonitorDialog;
import org.omnetpp.scave.model.Chart;

class ChartScriptDocumentProvider extends AbstractDocumentProvider {
    IDocument doc = null;

    public ProjectionAnnotationModel annotationModel = new ProjectionAnnotationModel();

    @Override
    protected IRunnableContext getOperationRunner(IProgressMonitor monitor) {
        return new TimeTriggeredProgressMonitorDialog(Display.getCurrent().getActiveShell(), 1000);
    }

    @Override
    public boolean isModifiable(Object element) {
        return true;
    }

    @Override
    public boolean isReadOnly(Object element) {
        return false;
    }

    @Override
    public boolean canSaveDocument(Object element) {
        return true;
    }

    @Override
    protected void doSaveDocument(IProgressMonitor monitor, Object element, IDocument document, boolean overwrite)
            throws CoreException {
        Chart chart = ((ChartScriptEditorInput)element).chart;
        chart.setScript(document.get());
    }

    @Override
    protected IDocument createDocument(Object element) throws CoreException {
        return getDocument(element);
    }

    @Override
    public IDocument getDocument(Object element) {
        Chart chart = ((ChartScriptEditorInput) element).chart;

        if (doc == null)
            doc = new Document(chart.getScript());

        return doc;
    }

    @Override
    protected IAnnotationModel createAnnotationModel(Object element) throws CoreException {
        return annotationModel;
    }
}