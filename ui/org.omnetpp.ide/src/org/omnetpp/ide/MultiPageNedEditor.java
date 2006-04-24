package org.omnetpp.ide;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned2.model.ModelUtil;
import org.omnetpp.ned2.model.NedFileNodeEx;
import org.omnetpp.resources.NEDResources;
import org.omnetpp.resources.NEDResourcesPlugin;

/**
 * Multi-page NED editor.
 * 
 * TODO react to resource changes (reread if file changed, close if file deleted) 
 */
public class MultiPageNedEditor extends MultiPageEditorPart implements
		IResourceChangeListener {

    private GraphicalNedEditor graphEditor;
	private TextualNedEditor nedEditor;
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
        super.dispose();
        NEDResourcesPlugin.getNEDResources().disconnect(((IFileEditorInput)getEditorInput()).getFile());
    }

	@Override
	protected void createPages() {
		graphEditor = new GraphicalNedEditor();
        graphEditor.setEmbeddingEditor(this);
		nedEditor = new TextualNedEditor();
        IFile ifile = ((FileEditorInput)getEditorInput()).getFile();
        
		try {
            // fill graphical editor
			NEDResources res = NEDResourcesPlugin.getNEDResources();
            NedFileNodeEx modelRoot = (NedFileNodeEx)res.getNEDFileContents(ifile);
            graphEditor.setModel(modelRoot);

            // fill text editor
            graphPageIndex = addPage(graphEditor, getEditorInput());
            setPageText(graphPageIndex,"Graphical");

            textPageIndex = addPage(nedEditor, getEditorInput());
            setPageText(textPageIndex,"Text");

            // only start in graphics mode if there's no error in the file
            if (!res.containsNEDErrors(ifile))
                setActivePage(graphPageIndex);
            
		} catch (PartInitException e) {
			e.printStackTrace();  //XXX handle it? let it propagate?
		}
	}
	
	@Override
	protected void pageChange(int newPageIndex) {
		super.pageChange(newPageIndex);
		
        //	prevent recursive call from setActivePage() below
		if (insidePageChange)
			return;
		insidePageChange = true;

		NEDResources res = NEDResourcesPlugin.getNEDResources();
		if (newPageIndex == textPageIndex) { 
			// switch from graphics to text:
			// generate text representation from the model
			NedFileNodeEx modelRoot = graphEditor.getModel();
            IFile ifile = ((FileEditorInput)getEditorInput()).getFile();
            
            // put the actual model state back to the incremental builder
            res.setNEDFileContents(ifile, modelRoot);
            
            // generate the text representation
            String textEditorContent = ModelUtil.generateNedSource(modelRoot, false);
            // put it into the text editor
            nedEditor.setText(textEditorContent);
		} 
		else if (newPageIndex == graphPageIndex) { 
			// switch from text to graphics
            IFile ifile = ((FileEditorInput)getEditorInput()).getFile();
            res.setNEDFileContents(ifile, nedEditor.getText());

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
				
				//XXX old code: ask user what to do
				//    if (graphEditor.getModel() != null) {
				//        MessageBox messageBox = new MessageBox(getEditorSite().getShell(), 
				//                SWT.ICON_WARNING | SWT.YES | SWT.NO);
				//        messageBox.setText("Warning");
				//        messageBox.setMessage("The editor contents has syntax errors, "+
				//                "switching is only possible with losing text mode changes. "+
				//        "Do you want to revert to graphics view (and lose your changes)?"); 
				//        // XXX better dialog, with "Continue editing" and "Lose changes" buttons
				//        int buttonID = messageBox.open();
				//        if (buttonID==SWT.YES) 
				//            setActivePage(graphPageIndex);
				//    } else {

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
	public Object getAdapter(Class type) {
		Object adapter = getActiveEditor().getAdapter(type);
		if (adapter == null) 
			adapter = super.getAdapter(type);
		return adapter;
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		graphEditor.doSave(monitor);
		nedEditor.doSave(monitor);
	}

	@Override
	public void doSaveAs() {
		// TODO open SaveAs dialog
		graphEditor.doSaveAs();
		nedEditor.doSaveAs();
	}

	@Override
	public boolean isSaveAsAllowed() {
		return true;
	}

	public void resourceChanged(IResourceChangeEvent event) {
		// TODO implement content ReGet from the incremental builder
        // or close the editor if the file was deleted
	}

}
