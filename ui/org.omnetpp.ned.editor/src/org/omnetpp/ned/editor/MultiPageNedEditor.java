package org.omnetpp.ned.editor;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.eclipse.ui.views.contentoutline.ContentOutline;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.graph.misc.ISelectionSupport;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.NedFileNodeEx;
import org.omnetpp.ned.resources.NEDResources;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * Multi-page NED editor.
 * 
 * TODO react to resource changes (reread if file changed, close if file deleted)
 * FIXME File|Open in Eclipse won't work!!! it creates a JavaFileEditorInput which is NOT an IFileEditorInput!!! 
 */
public class MultiPageNedEditor extends MultiPageEditorPart implements
		IResourceChangeListener, ISelectionSupport {

    private GraphicalNedEditor graphEditor;
	private TextualNedEditor nedEditor;
    private String textFormat = "";          // the text version of the file the last time we have switched editors
	private int graphPageIndex;
	private int textPageIndex;
	private boolean insidePageChange = false;
    private boolean initPhase = true;
	
	public void init(IEditorSite site, IEditorInput editorInput) throws PartInitException {
		super.init(site, editorInput);
		setPartName(editorInput.getName());
        if (!(editorInput instanceof IFileEditorInput))
            throw new PartInitException("Invalid input type!");
        
        NEDResourcesPlugin.getNEDResources().connect(((IFileEditorInput)editorInput).getFile());
	}
	
    
    @Override
    public void dispose() {
        // TODO maybe not the dispose is the righ place to disconnect (rather when the editor is closed)
        super.dispose();
        NEDResourcesPlugin.getNEDResources().disconnect(((IFileEditorInput)getEditorInput()).getFile());
    }

	@Override
	protected void createPages() {
		graphEditor = new GraphicalNedEditor();
		nedEditor = new TextualNedEditor();
        IFile ifile = ((FileEditorInput)getEditorInput()).getFile();
        
		try {
            // setup graphical editor
			NEDResources res = NEDResourcesPlugin.getNEDResources();
            NedFileNodeEx modelRoot = (NedFileNodeEx)res.getNEDFileContents(ifile);
            graphEditor.setModel(modelRoot);
            graphPageIndex = addPage(graphEditor, getEditorInput());
            setPageText(graphPageIndex,"Graphical");

            // setup text editor
            // we don't have to set the content because it's set 
            // automatically by the text editor (from the FileEditorInput)
            textPageIndex = addPage(nedEditor, getEditorInput());
            setPageText(textPageIndex,"Text");

            // switch to graphics mode initially if there's no error in the file
            if (!res.containsNEDErrors(ifile))
                setActivePage(graphPageIndex);
            
		} catch (PartInitException e) {
			e.printStackTrace();  //XXX handle it? let it propagate?
		}
	}
	
	@Override
	protected void pageChange(int newPageIndex) {
	    //	prevent recursive call from setActivePage() below
	    if (insidePageChange)
	        return;
	    
	    insidePageChange = true;

        super.pageChange(newPageIndex);

        // XXX this is a MEGA hack because currently the workbench do not send a partActivated,deactivated messge
        // for the embedded editors in a MultiPageEditorView (this is a missing unimplemented feature, it works with MultiEditor however)
        // to make the nedded outline page active we should send activate/deactivate directly
        // we look for the outline view directy and send thenotification by hand. once the MultiPageEditors are handled correctly
        // this can be removed
        // on each page change we emulate a close/open cycle of the multipage editor, this removed the associated
        // outline page, so the outline view will re-request the multipageeditor for a ContentOutlinePage (via getAdapter)
        // the current implementation of MultipageEditorPart.getAdapter delegates this request to the active 
        // embedded editor.
        ContentOutline coutline = (ContentOutline)getEditorSite().getPage().findView("org.eclipse.ui.views.ContentOutline");
        if (coutline != null) {
            // notify from the old closed editor
            coutline.partClosed(this);
            coutline.partActivated(this);
        }
        // end of the hack

		NEDResources res = NEDResourcesPlugin.getNEDResources();
		if (newPageIndex == textPageIndex) { 
			// switch from graphics to text:
			// generate text representation from the model
			NedFileNodeEx modelRoot = graphEditor.getModel();
            // generate the text representation
            textFormat = NEDTreeUtil.generateNedSource(modelRoot, true);
            // put it into the text editor if changed
            if (!textFormat.equals(nedEditor.getText()))
                nedEditor.setText(textFormat);
		} 
		else if (newPageIndex==graphPageIndex && !textFormat.equals(nedEditor.getText())) {
			// switch from text to graphics
		    IFile ifile = ((FileEditorInput)getEditorInput()).getFile();
            textFormat = nedEditor.getText();
            res.setNEDFileContents(ifile, textFormat);
            // convert it to object representation
            NedFileNodeEx modelRoot = (NedFileNodeEx)res.getNEDFileContents(ifile);
            // only start in graphics mode if there's no error in the file
            if (!res.containsNEDErrors(ifile)) {
				// give the backparsed model to the graphical editor 
				graphEditor.setModel(modelRoot);
			}
			else {
                // this happens if the parsing was unsuccessful when we wanted to switch from text to graph mode
				// parse error: switch back immediately to text view (we should never have 
				// switched away from it in the first place)
				setActivePage(textPageIndex);
				
				if (!initPhase) {
					MessageBox messageBox = new MessageBox(getEditorSite().getShell(), SWT.ICON_WARNING | SWT.OK);
					messageBox.setText("Warning");
					messageBox.setMessage("The editor contents has errors, switching is not possible. "+
					                      "Please fix the errors first.");
					messageBox.open();
	            }
			}
		}
		insidePageChange = false;
        initPhase = false;
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		if (getActivePage() == graphPageIndex) { 
			// switch from graphics to text:
			// generate text representation from the model
			NedFileNodeEx modelRoot = graphEditor.getModel();
            IFile ifile = ((FileEditorInput)getEditorInput()).getFile();
            
            // put the actual model state back to the incremental builder
    		NEDResourcesPlugin.getNEDResources().setNEDFileContents(ifile, modelRoot);
            
            // generate the text representation
            String textEditorContent = NEDTreeUtil.generateNedSource(modelRoot, true);
            // put it into the text editor
            nedEditor.setText(textEditorContent);
		}
		// delegate the save task to the TextEditor's save method
		nedEditor.doSave(monitor);
	}

	@Override
	public void doSaveAs() {
		// TODO open SaveAs dialog
		getActiveEditor().doSaveAs();
	}

	@Override
	public boolean isSaveAsAllowed() {
		return true;
	}

	public void resourceChanged(IResourceChangeEvent event) {
		// TODO implement content ReGet from the incremental builder
        // or close the editor if the file was deleted
	}

    public void selectComponent(String componentName) {
        graphEditor.selectComponent(componentName);
    }

}
