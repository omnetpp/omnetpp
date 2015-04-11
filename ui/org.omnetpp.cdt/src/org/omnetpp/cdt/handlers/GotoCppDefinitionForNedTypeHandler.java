/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

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
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.cdt.Activator;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedModelProvider;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.ChannelElement;
import org.omnetpp.ned.model.pojo.SimpleModuleElement;

/**
 * Goes to a C++ definition of a NED simple module or channel.
 *
 * @author Levy, andras, rhornig
 */
@SuppressWarnings("restriction")
public class GotoCppDefinitionForNedTypeHandler extends AbstractHandler {

    @Override
    public boolean isEnabled() {
        return super.isEnabled() && getNedElement()!=null;
    }

    protected INedTypeElement getNedElement() {
        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        IWorkbenchPage page = window == null ? null : window.getActivePage();

        if (page != null) {
            ISelection selection = page.getSelection();

            if (selection instanceof IStructuredSelection) {

                IStructuredSelection structuredSelection = (IStructuredSelection)selection;
                INedElement nedElement = null;

                Object element = structuredSelection.getFirstElement();
                if (element instanceof INedElement)
                    nedElement = (INedElement)element;

                if (element instanceof INedModelProvider)
                    nedElement = ((INedModelProvider)element).getModel();

                // If we have a selection on an inner element, find the first element up that may have a C++ implementation
                // or stop at the top level NedFileElement
                while (nedElement!=null && !(nedElement instanceof INedTypeElement || nedElement instanceof ISubmoduleOrConnection
                                             || nedElement instanceof NedFileElementEx))
                    nedElement = nedElement.getParent();

                // if the whole file is selected find the first simple module element that may
                // have C++ implementation. Or try a Channel if no simple module exist in the file
                if (nedElement instanceof NedFileElementEx) {
                    INedElement implicitSelection = ((NedFileElementEx)nedElement).getFirstSimpleModuleChild();
                    if (implicitSelection == null)
                        implicitSelection = ((NedFileElementEx)nedElement).getFirstChannelChild();

                    if (implicitSelection != null)
                        nedElement = implicitSelection;
                }

                if (nedElement instanceof ISubmoduleOrConnection)
                    nedElement = ((ISubmoduleOrConnection)nedElement).getEffectiveTypeRef();

                if (nedElement instanceof SimpleModuleElement || nedElement instanceof ChannelElement) {
                    INedTypeElement nedTypeElement = (INedTypeElement)nedElement;
                    if (nedTypeElement.getNedTypeInfo().getProject() != null) // not a built-in type
                        return nedTypeElement;
                }
            }
        }
        return null;
    }

    public Object execute(ExecutionEvent event) throws ExecutionException {
        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        IWorkbenchPage page = window == null ? null : window.getActivePage();
        INedTypeElement nedTypeElement = getNedElement();

        if (page != null && nedTypeElement != null) {
            String className = nedTypeElement.getNedTypeInfo().getFullyQualifiedCppClassName();
            IProject project = nedTypeElement.getNedTypeInfo().getProject();
            Assert.isNotNull(project);

            IProject[] projects;
            try {
                projects = ProjectUtils.getAllReferencedProjects(project, false, true);
            }
            catch (CoreException e) {
                throw new ExecutionException("Error getting list of referenced projects", e);
            }

            if (!gotoCppDefinition(page, projects, className)) {
                if (page.getActiveEditor()!=null && page.getActiveEditor().getEditorInput() instanceof FileEditorInput) {
                    IFile editedFile = ((FileEditorInput)page.getActiveEditor().getEditorInput()).getFile();
                    if(!openNearbyCppFile(page, editedFile)) {
                        MessageDialog.openError(window.getShell(), "Not Found", "C++ class '"+className+"' not found in project '"+project.getName()+"' and its referenced projects.");
                    }

                }
            }
        }
        return null;
    }

    protected boolean openNearbyCppFile(IWorkbenchPage page, IFile inputFile) {
        IContainer container = inputFile.getParent();
        IFile hFile = container.getFile(new Path(inputFile.getName()).removeFileExtension().addFileExtension(".h"));
        IFile ccFile = container.getFile(new Path(inputFile.getName()).removeFileExtension().addFileExtension(".cc"));
        IFile file = null;

        if (hFile.exists())
            file = hFile;
        else if (ccFile.exists())
            file = ccFile;

        if (file != null) {
            try {
                IDE.openEditor(page, file, true);
                return true;
            }
            catch (PartInitException e) {
                Activator.logError("Could not open: "+file.toString(),e);
            }
        }
        return false;
    }

    private boolean gotoCppDefinition(IWorkbenchPage page, IProject[] projects, String qualifiedClassName) {
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
                                if (editor != null) {
                                    IASTFileLocation location = name.getFileLocation();
                                    ISourceRange range = new SourceRange(location.getNodeOffset(), location.getNodeLength());
                                    editor.setSelection(range, true);
                                }
                                return true;
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

        return false;
    }
}
