package org.omnetpp.cdt.handlers;

import org.eclipse.cdt.core.CCorePlugin;
import org.eclipse.cdt.core.dom.ast.IASTFileLocation;
import org.eclipse.cdt.core.dom.ast.IBinding;
import org.eclipse.cdt.core.dom.ast.cpp.ICPPClassType;
import org.eclipse.cdt.core.index.IIndex;
import org.eclipse.cdt.core.index.IIndexFile;
import org.eclipse.cdt.core.index.IIndexManager;
import org.eclipse.cdt.core.index.IIndexName;
import org.eclipse.cdt.core.index.IndexFilter;
import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.model.ICProject;
import org.eclipse.cdt.core.model.ISourceRange;
import org.eclipse.cdt.internal.core.model.ext.SourceRange;
import org.eclipse.cdt.internal.ui.editor.CEditor;
import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.editor.NedEditor;
import org.omnetpp.ned.editor.graph.edit.NedTypeEditPart;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

@SuppressWarnings("restriction")
public class GotoCppDefinitionForNEDTypeHandler extends AbstractHandler {
    public Object execute(ExecutionEvent event) throws ExecutionException {
        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        
        if (window != null) {
            IWorkbenchPage page = window.getActivePage();
            
            if (page != null) {
                ISelection selection = page.getSelection();
        
                if (selection instanceof IStructuredSelection && page.getActiveEditor() instanceof NedEditor) {
                    NedEditor nedEditor = (NedEditor)page.getActiveEditor();
                    IStructuredSelection structuredSelection = (IStructuredSelection)selection;
                    INedTypeElement nedTypeElement = null;

                    if (structuredSelection.getFirstElement() instanceof INedTypeElement)
                        nedTypeElement = (INedTypeElement)structuredSelection.getFirstElement();

                    if (structuredSelection.getFirstElement() instanceof NedTypeEditPart)
                        nedTypeElement = (INedTypeElement)((NedTypeEditPart)structuredSelection.getFirstElement()).getNedModel();

                    if (nedTypeElement != null) {
                        String className = nedTypeElement.getNEDTypeInfo().getFullyQualifiedCppClassName();
                        IProject[] projects = ProjectUtils.getAllReferencedProjects(nedEditor.getFile().getProject(), true);
                        gotoCppDefinition(page, projects, className);
                    }
                }
            }
        }

        return null;
    }
    
    private void gotoCppDefinition(IWorkbenchPage page, IProject[] projects, String qualifiedClassName) {
        IIndexManager manager = CCorePlugin.getIndexManager();

        for (IProject project : projects) {
            IIndex index = null;
            try {
                ICProject cProject = CoreModel.getDefault().getCModel().getCProject(project.getName());
                index = manager.getIndex(cProject);
                index.acquireReadLock();

                int lastIndex = qualifiedClassName.lastIndexOf("::");
                String className = lastIndex == -1 ? qualifiedClassName : qualifiedClassName.substring(lastIndex + 2);
                IBinding[] bindings = index.findBindings(className.toCharArray(), false, IndexFilter.ALL, null);

                for (IBinding binding : bindings) {
                    if (binding instanceof ICPPClassType) {
                        ICPPClassType cppClass = (ICPPClassType)binding;
                        if (qualifiedClassName.equals(StringUtils.join(cppClass.getQualifiedName(), "::"))) {
                            IIndexName[] names = index.findNames(cppClass, IIndex.FIND_DEFINITIONS);
                    
                            for (IIndexName name : names) {
                                IIndexFile indexFile = name.getFile();
                                IFile file = project.getWorkspace().getRoot().getFile(new Path(indexFile.getLocation().getFullPath()));
                                CEditor editor = (CEditor)IDE.openEditor(page, file, true);
                                IASTFileLocation location = name.getFileLocation();
                                ISourceRange range = new SourceRange(location.getNodeOffset(), location.getNodeLength()); 
                                editor.setSelection(range, true);
                                return;
                            }
                        }
                    }
                }
            }
            catch (Exception e) {
                throw new RuntimeException(e);
            }
            finally  {
              if (index != null)
                  index.releaseReadLock();
            }
        }
    }
}
