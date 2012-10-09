package org.omnetpp.common.wizard;

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;

import freemarker.cache.TemplateLoader;

/**
 * FreeMarker template loader that loads templates from the Eclipse workspace.
 *
 * @author Andras
 */
//TODO find a solution for reading templates outside the templatefolder... (FreeMarker rejects template names that begin with "../")
public class WorkspaceTemplateLoader implements TemplateLoader {
    IFolder templateFolder;

    public WorkspaceTemplateLoader(IFolder templateFolder) {
        this.templateFolder = templateFolder;
    }

    public Object findTemplateSource(String name) throws IOException {
        IFile file = templateFolder.getFile(new Path(name));
        return file.exists() ? file : null;
    }

    public void closeTemplateSource(Object templateSource) throws IOException {
        // nothing
    }

    public Reader getReader(Object templateSource, String encoding) throws IOException {
        try {
            return new InputStreamReader(((IFile)templateSource).getContents(true));
        } catch (CoreException e) {
            IOException ioException = new IOException("Cannot read template file " + ((IFile)templateSource).getFullPath());
            ioException.initCause(e);
            throw ioException;
        }
    }

    public long getLastModified(Object templateSource) {
        return ((IFile)templateSource).getModificationStamp();
    }

}
