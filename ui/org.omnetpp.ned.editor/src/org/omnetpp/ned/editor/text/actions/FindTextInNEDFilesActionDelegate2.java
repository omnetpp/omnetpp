package org.omnetpp.ned.editor.text.actions;


import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.search.internal.ui.text.FileSearchQuery;
import org.eclipse.search.ui.ISearchQuery;
import org.eclipse.search.ui.text.FileTextSearchScope;
import org.eclipse.search.ui.text.TextSearchQueryProvider;
import org.eclipse.search2.internal.ui.text2.FindInRecentScopeActionDelegate;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.ned.editor.MultiPageNedEditor;


/**
 * Modification of the standard "Search Text in Workspace" action.
 *   1. search only in NED files
 *   2. search word under cursor if nothing is selected (i.e. no need to select word in advance)
 *    
 * @author andras
 */
@SuppressWarnings("restriction")
public class FindTextInNEDFilesActionDelegate2 extends FindInRecentScopeActionDelegate {

    public FindTextInNEDFilesActionDelegate2() {
		super("Search word in NED files");
		setActionDefinitionId("org.eclipse.search.ui.performTextSearchWorkspace"); //FIXME
	}

    @Override
    protected String getSearchForString(IWorkbenchPage page) {
        String searchString = super.getSearchForString(page);
        
        // Issue: super.getSearchForString() doesn't work 100% with the NED editor:
        // doesn't do anything when the text selection is empty (it doesn't search for  
        // word under cursor). This is because the active editor is the multi-page editor 
        // not the text editor, and it returns selection==null. 
        // The following is a workaround.
        //
        if (searchString.equals("")) {
            IWorkbenchPart activePart = page.getActivePart();
            if (activePart instanceof MultiPageNedEditor) {
                MultiPageNedEditor multiPageEditor = (MultiPageNedEditor)activePart;
                if (multiPageEditor.isActiveEditor(multiPageEditor.getTextEditor())) {
                    searchString = extractSearchTextFromEditor(multiPageEditor.getTextEditor());
                }
            }
        }
        return searchString;
    }

    @Override
	protected ISearchQuery createQuery(TextSearchQueryProvider provider, String searchForString) throws CoreException {
        // we only want to search in NED files. Ignore the provider, and create the query ourselves.
        FileTextSearchScope scope = FileTextSearchScope.newSearchScope(
                new IResource[] {ResourcesPlugin.getWorkspace().getRoot()}, 
                new String[] { ".ned" }, 
                true); 
        //FIXME make "whole words only" via regex!!
        return new FileSearchQuery(searchForString, false, false, scope);
	}
}
