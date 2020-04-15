package freemarker.eclipse.editors;

import java.io.Reader;
import java.io.StringReader;
import java.util.Enumeration;
import java.util.Vector;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.reconciler.DirtyRegion;
import org.eclipse.jface.text.reconciler.IReconcilingStrategy;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IFileEditorInput;

import freemarker.core.ParseException;
import freemarker.core.TemplateElement;
import freemarker.template.Configuration;
import freemarker.template.Template;


/**
 * @version $Id: ReconcilingStrategy.java,v 1.4 2003/08/15 19:49:08 stephanmueller Exp $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 */
public class ReconcilingStrategy implements IReconcilingStrategy {
    private FreemarkerEditor fEditor;
    private Template fTemplate;
    //private Template fLastTemplate;
    private String fError;

    public ReconcilingStrategy(FreemarkerEditor anEditor) {
        fEditor = anEditor;
    }

    public void setDocument(IDocument aDocument) {
        parse();
    }

    public void reconcile(DirtyRegion aDirtyRegion, IRegion aRegion) {
        parse();
    }

    public void reconcile(IRegion aPartition) {
        parse();
    }

    private void parse() {
        IFile file = ((IFileEditorInput)fEditor.getEditorInput()).getFile();
        Template template = null;
        int fErrorLine = 0;
        try {
            file.deleteMarkers(IMarker.PROBLEM, true, IResource.DEPTH_INFINITE);
            Reader reader = new StringReader(fEditor.getDocument().get());
            Configuration cfg = new Configuration();
            template = new Template(file.getName(), reader, cfg);
            reader.close();
            fError = "";
        } catch (ParseException e) {
            if (e.getMessage() != null) {
                fError = e.getMessage();
                try {
                    fErrorLine = e.getLineNumber();
                } catch (NullPointerException npe) {
                    fErrorLine = 0;
                }

            } else {
                fError = "";
            }
        } catch (Exception e) {

        }
        synchronized (this) {
            if (template != null) {
                fTemplate = template;

                // Save last successful parse tree
                //fLastTemplate = template;

            } else {
                fTemplate = null;
                try {
                fEditor.addProblemMarker(fError,fErrorLine);
                } catch (Exception e) {

                }
            }
        }

        // Update outline view and display error message in status line
        Display.getDefault().syncExec(new Runnable() {
            public void run(){
                fEditor.updateOutlinePage();
                fEditor.displayErrorMessage(fError);
            }
        });
    }

    public Object[] getTemplateRoot() {
        if(null==fTemplate) {
            return new Object[0];
        } else {
            if("root element".equals(fTemplate.getRootTreeNode().getDescription())) {
                return enum2array(fTemplate.getRootTreeNode().children());
            } else {
                return new Object[]{fTemplate.getRootTreeNode()};
            }
        }
    }

    public Template getTemplate() {
        return fTemplate;
    }

    private Object[] enum2array(Enumeration<TemplateElement> e) {
        Vector<TemplateElement> temp = new Vector<TemplateElement>();
        TemplateElement t;
        while(e.hasMoreElements()) {
            t = e.nextElement();
            //if(!"whitespace".equalsIgnoreCase(t.getDescription())) {
                temp.add(t);
            //}
        }
        return temp.toArray();
    }
}

