package org.omnetpp.cdt.cache;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.cdt.core.index.IIndexFileLocation;
import org.eclipse.cdt.core.parser.FileContent;
import org.eclipse.cdt.core.parser.ParserUtil;
import org.eclipse.cdt.internal.core.parser.scanner.AbstractCharArray;
import org.eclipse.cdt.internal.core.parser.scanner.CharArray;
import org.eclipse.cdt.internal.core.parser.scanner.InternalFileContent;
import org.eclipse.cdt.internal.core.parser.scanner.InternalFileContent.InclusionKind;
import org.eclipse.cdt.internal.core.parser.scanner.InternalFileContentProvider;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.build.MakefileTools;
import org.omnetpp.common.Debug;

/**
 * This class stores a "compressed" format of C++ source and header files.
 * The "compressed" format contains only the preprocessing directives.
 * This format can be parsed faster than the complete source file, still
 * contains all the information needed to generate the dependency information.
 * <p>
 * The cache is updated when the resource is changed. This class can notify
 * listeners about resource changes, which caused a change in the compressed
 * format of the file.
 * <p>
 * Note: If an include file is requested more than once for a translation unit,
 * the file content is returned only for the first query, subsequent queries
 * will receive a skipped content. Here we assume that all header files contains
 * the necessary top level #ifdefs to prevent their content included twice.
 * This improves scanning time of large projects tremendously.
 */
@SuppressWarnings("restriction")
public class CachedFileContentProvider extends InternalFileContentProvider {

    // standard headers are always skipped
    protected static final Set<String> standardHeaders = new HashSet<String>(Arrays.asList(MakefileTools.ALL_STANDARD_HEADERS.trim().split(" +")));

    // map of file locations to file content
    Map<String, InternalFileContent> cache = new HashMap<String, InternalFileContent>();

    // files included into the current translation unit
    // their path is mapped to a skipped content (kind == SKIP_FILE)
    Map<String, InternalFileContent> includedFiles = new HashMap<String, InternalFileContent>();

    // listeners to be notified if a file content changed
    ListenerList listeners = new ListenerList();

    private IResourceChangeListener resourceChangeListener = new IResourceChangeListener() {
        public void resourceChanged(IResourceChangeEvent event) {
            CachedFileContentProvider.this.resourceChanged(event);
        }
    };

    public CachedFileContentProvider() {
    }

    public void hookListeners() {
        ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceChangeListener);
    }

    public void unhookListeners() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceChangeListener);
    }

    public InternalFileContent getContentForTranslationUnit(String path) {
        includedFiles.clear();
        return getFileContent(path);
    }

    @Override
    public InternalFileContent getContentForInclusion(String path) {
        InternalFileContent content = includedFiles.get(path);
        if (content != null)
            return content;

        content = getFileContent(path);

        if (content != null)
            includedFiles.put(path, new InternalFileContent(path, InclusionKind.SKIP_FILE));

        return content;
    }

    @Override
    public InternalFileContent getContentForInclusion(IIndexFileLocation ifl, String astPath) {
        throw new UnsupportedOperationException();
    }

    private InternalFileContent getFileContent(String path) {
        if (cache.containsKey(path))
            return cache.get(path);

        InternalFileContent fileContent = null;
        if (standardHeaders.contains(new Path(path).lastSegment()))
            fileContent = new InternalFileContent(path, InclusionKind.SKIP_FILE);

        if (fileContent == null) {
            AbstractCharArray content = readAndCompressFileContent(path);
            if (content != null)
                fileContent = new InternalFileContent(path, content);
        }

        cache.put(path, fileContent);
        return fileContent;
    }

    public AbstractCharArray readAndCompressFileContent(String path) {
        if (!getInclusionExists(path))
            return null;

        //Debug.println(path);
        IResource file = ParserUtil.getResourceForFilename(path);
        FileContent content = file instanceof IFile ?
                              FileContent.create((IFile) file) :
                              FileContent.createForExternalFileLocation(path);

        return content instanceof InternalFileContent ?
               compressFileContent(((InternalFileContent)content).getSource()) : null;
    }

    private final static Pattern preprocessorDirective = Pattern.compile("^[ \t]*#[ \t]*([a-zA-Z_]+)[ \t]*?(.*)$", Pattern.MULTILINE);
    private final static Pattern blockComment = Pattern.compile("/\\*(.*?)\\*/", Pattern.DOTALL);

    private AbstractCharArray compressFileContent(AbstractCharArray content) {
        char[] chars = new char[content.getLength()];
        content.arraycopy(0, chars, 0, chars.length);
        String str = new String(chars);

        // merge \ continuation lines
        str = str.replaceAll("\\\\\\n", "");
        // replace /* ... */ comments with one space
        str = blockComment.matcher(str).replaceAll(" ");
        // extract preprocessor macros
        StringBuilder sb = new StringBuilder();
        Matcher matcher = preprocessorDirective.matcher(str);
        while (matcher.find()) {
            sb.append("#").append(matcher.group(1)).append(" ").append(matcher.group(2)).append("\n");
        }
        //Debug.println(sb.toString());
        return new CharArray(sb.toString());
    }

    protected void resourceChanged(IResourceChangeEvent event) {
        try {
            // refresh the cache on resource change events
            if (event.getDelta() != null) {
                event.getDelta().accept(new IResourceDeltaVisitor() {
                    public boolean visit(IResourceDelta delta) throws CoreException {
                        IResource resource = delta.getResource();
                        if (MakefileTools.isCppFile(resource) || MakefileTools.isMsgFile(resource)) {
                            int kind = delta.getKind();
                            int flags = delta.getFlags();
                            String path = resource.getLocation().toOSString(); // XXX normalized?
                            if (kind == IResourceDelta.ADDED) {
                                AbstractCharArray content = readAndCompressFileContent(path);
                                cache.put(path, new InternalFileContent(path, content));
                                fireFileContentChanged(delta);
                            }
                            else if (kind == IResourceDelta.REMOVED) {
                                InternalFileContent oldContent = cache.remove(path);
                                if (oldContent != null)
                                    fireFileContentChanged(delta);
                            }
                            else if (kind==IResourceDelta.CHANGED && (flags&IResourceDelta.CONTENT)!=0) {
                                InternalFileContent fileContent = cache.get(path);
                                if (fileContent != null && fileContent.getKind() == InclusionKind.USE_SOURCE) {
                                    AbstractCharArray oldContent = fileContent.getSource();
                                    AbstractCharArray newContent = readAndCompressFileContent(path);
                                    boolean contentChanged = oldContent.getLength() != newContent.getLength() ||
                                                             oldContent.getContentsHash() != newContent.getContentsHash();
                                    if (!contentChanged) {
                                        for (int i = 0; i < oldContent.getLength(); ++i) {
                                            if (oldContent.get(i) != newContent.get(i)) {
                                                contentChanged = true;
                                                break;
                                            }
                                        }
                                    }
                                    if (contentChanged) {
                                        cache.put(path, new InternalFileContent(path, newContent));
                                        fireFileContentChanged(delta);
                                    }
                                }
                                else
                                    fireFileContentChanged(delta);
                            }
                        }
                        return true;
                    }
                });
            }
        }
        catch (CoreException e) {
            Activator.logError(e);
        }
    }

    static interface ISourceFileChangeListener {
        void sourceFileChanged(IResourceDelta delta);
    }

    void addFileChangeListener(ISourceFileChangeListener listener) {
        listeners.add(listener);
    }

    void removeFileChangeListener(ISourceFileChangeListener listener) {
        listeners.remove(listener);
    }

    protected void fireFileContentChanged(IResourceDelta delta) {
        for (Object listener : listeners.getListeners()) {
            ((ISourceFileChangeListener)listener).sourceFileChanged(delta);
        }
    }
}
