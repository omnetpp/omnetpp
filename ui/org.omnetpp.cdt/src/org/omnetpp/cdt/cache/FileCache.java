package org.omnetpp.cdt.cache;

import java.io.File;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.cdt.core.parser.FileContent;
import org.eclipse.cdt.core.parser.ParserUtil;
import org.eclipse.cdt.internal.core.parser.scanner.AbstractCharArray;
import org.eclipse.cdt.internal.core.parser.scanner.CharArray;
import org.eclipse.cdt.internal.core.parser.scanner.InternalFileContent;
import org.eclipse.cdt.internal.core.parser.scanner.InternalFileContent.InclusionKind;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.build.MakefileTools;

/**
 * This class stores a "compressed" format of C++ source (.cc, .h) and message files.
 * The "compressed" format contains only the preprocessing directives.
 * This format can be parsed faster than the complete source file, still
 * contains all the information needed to generate the dependency information.
 * <p>
 * The cache is updated when the resource is changed. This class can notify
 * listeners about resource changes, which caused a change in the compressed
 * format of the file.
 *
 * @author tomi
 */
@SuppressWarnings("restriction")
public class FileCache {

    // relative paths of standard headers grouped by their last segment
    protected static final Map<String,IPath[]> standardHeaders;
    static {
        standardHeaders = new HashMap<String,IPath[]>();
        for (String header : Arrays.asList(MakefileTools.ALL_STANDARD_HEADERS.trim().split(" +"))) {
            IPath path = new Path(header);
            IPath[] oldGroup = standardHeaders.get(path.lastSegment());
            IPath[] group;
            if (oldGroup == null)
                group = new IPath[] { path };
            else {
                group = new IPath[oldGroup.length + 1];
                System.arraycopy(oldGroup, 0, group, 0, oldGroup.length);
                group[group.length-1] = path;
            }
            standardHeaders.put(path.lastSegment(), group);
        }
    }


    // map of file locations to file content
    // if the value is null, then the file does not exists in the file system
    protected Map<String, InternalFileContent> cache = new HashMap<String, InternalFileContent>();

    // listeners to be notified if a file content changed
    private ListenerList listeners = new ListenerList();

    private IResourceChangeListener resourceChangeListener = new IResourceChangeListener() {
        public void resourceChanged(IResourceChangeEvent event) {
            FileCache.this.resourceChanged(event);
        }
    };

    public FileCache() {
    }

    public void hookListeners() {
        ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceChangeListener);
    }

    public int getNumberOfFiles() {
        return cache.size();
    }

    public void unhookListeners() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceChangeListener);
    }

    public static boolean isStandardHeader(String path) {
        IPath headerPath = new Path(path);
        IPath[] standardHeaderPaths = standardHeaders.get(headerPath.lastSegment());
        if (standardHeaderPaths != null) {
            for (IPath standardHeaderPath : standardHeaderPaths) {
                // check that headerPath ends with the segments of standardHeaderPath.
                // we know that the last segments are equal, so they are not compared
                int offset = headerPath.segmentCount() - standardHeaderPath.segmentCount();
                if (offset >= 0) {
                    int i;
                    for (i = standardHeaderPath.segmentCount() - 2; i >= 0; --i)
                        if (!standardHeaderPath.segment(i).equals(headerPath.segment(i+offset)))
                            break;
                    if (i < 0)
                        return true;
                }
            }
        }
        return false;
    }

    public InternalFileContent getFileContent(String path) {
        if (cache.containsKey(path)) {
            return cache.get(path);
        }

        InternalFileContent fileContent = null;
        if (isStandardHeader(path))
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
        if (!new File(path).exists())
            return null;

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
                                InternalFileContent data = cache.get(path);
                                if (data != null && data.getKind() == InclusionKind.USE_SOURCE) {
                                    AbstractCharArray oldContent = data.getSource();
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
