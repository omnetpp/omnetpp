package org.omnetpp.ned.editor.text.actions;


import java.util.regex.Pattern;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.search.internal.ui.text.FileSearchQuery;
import org.eclipse.search.ui.ISearchQuery;
import org.eclipse.search.ui.text.FileTextSearchScope;
import org.eclipse.search.ui.text.TextSearchQueryProvider;
import org.eclipse.search2.internal.ui.text2.RetrieverAction;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.editor.text.NedSyntaxHighlightHelper;
import org.omnetpp.common.editor.text.TextEditorUtil;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.editor.MultiPageNedEditor;
import org.omnetpp.ned.editor.text.TextualNedEditor;


/**
 * Modification of the standard "Search Text in Workspace" action.
 * This is a temporary solution until we implement proper NED search.
 * Features:
 *   1. search only in NED files;
 *   2. search word under cursor if nothing is selected (i.e. no need to select word in advance);
 *   3. search for whole words only
 *    
 * @author andras
 */
@SuppressWarnings("restriction")
public class FindTextInNedFilesActionDelegate extends RetrieverAction {
    public static final String ID = "performTextSearchWorkspace";

    public FindTextInNedFilesActionDelegate() {
		setId(ID);
		setActionDefinitionId(NedTextEditorAction.ACTION_DEFINITION_PREFIX + ID);
		setText("Search Text in NED Files"); // looks like it's still needed for the context menu?
	}

    @Override
    public void run() {
        super.run();
    }

    @Override
    protected IWorkbenchPage getWorkbenchPage() {
        return PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
    }

    @Override
    protected String getSearchForString(IWorkbenchPage page) {
        String searchText = super.getSearchForString(page);
        
        // Issue: super.getSearchForString() doesn't work 100% with the NED editor:
        // it doesn't do anything when the text selection is empty (it doesn't search for  
        // word under cursor). This is because the multi-page editor (and BTW our text 
        // editor too) exports StructuredSelection not a TextSelection. 
        // The following is a workaround.
        //
        if (searchText.equals("")) {
            IWorkbenchPart activePart = page.getActivePart();
            if (activePart instanceof MultiPageNedEditor) {
                MultiPageNedEditor multiPageEditor = (MultiPageNedEditor)activePart;
                if (multiPageEditor.isActiveEditor(multiPageEditor.getTextEditor())) {
                    TextualNedEditor textEditor = (TextualNedEditor)multiPageEditor.getTextEditor();
                    ISourceViewer sourceViewer = textEditor.getSourceViewerPublic();
                    try {
                        searchText = TextEditorUtil.getWordRegion(sourceViewer, sourceViewer.getSelectedRange().x, new NedSyntaxHighlightHelper.NedWordDetector());
                    } 
                    catch (BadLocationException e) {
                    }
                }
            }
        }
        return searchText;
    }

    @Override
	protected ISearchQuery createQuery(TextSearchQueryProvider provider, final String searchText) throws CoreException {
        // we only want to search in NED files. Ignore the provider, and create the query ourselves.
        FileTextSearchScope scope = FileTextSearchScope.newSearchScope(
                new IResource[] {ResourcesPlugin.getWorkspace().getRoot()}, 
                new String[] { "*.ned" }, 
                true); 

        // turn it into "whole words only" search via regex
        return new FileSearchQuery("\\b" + escape(searchText) + "\\b", true, true, scope) {
            @Override
            public String getResultLabel(int matches) {
                // override label creation to hide ugly regex (still visible in the search dialog though)
                return "'" + searchText + "' - " + StringUtils.formatCounted(matches, "match") + " in workspace NED files";
            }
        };
	}

    protected String escape(String searchText) {
        return StringUtils.containsNone(searchText, "*?+.^$()[]{}|\\") ? searchText : Pattern.quote(searchText);
    }
}
