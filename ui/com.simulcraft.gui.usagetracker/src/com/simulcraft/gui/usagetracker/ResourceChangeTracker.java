package com.simulcraft.gui.usagetracker;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import org.eclipse.core.internal.events.ResourceDelta;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.util.StringUtils;

/**
 * Records workspace changes into the log.
 *   
 * @author Andras
 */
public class ResourceChangeTracker implements IResourceChangeListener {
    // if logFileContent is on, first X bytes of text files will be printed into the log
    protected boolean logFileContent = true;
    protected long numFirstBytesToLog = 16384;  //16K

    public ResourceChangeTracker() {
    }

    public void hookListeners() {
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this);
    }

    public void unhookListeners() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
    }

    public void resourceChanged(IResourceChangeEvent event) {
        final StringBuilder buffer = new StringBuilder();
        buffer.append("Resource change, type=" + eventTypeToString(event.getType()));
        if (event.getDelta() != null) {
            try {
                buffer.append("\n");
                event.getDelta().accept(new IResourceDeltaVisitor() {
                    public boolean visit(IResourceDelta delta) throws CoreException {
                        // skip empty "CHANGED" notifications (i.e. for folders when one file in it changes)
                        if (delta.getKind() != IResourceDelta.CHANGED || delta.getFlags() != 0) {
                            buffer.append("  " + ResourceChangeTracker.this.toString(delta));
                            if (delta.getResource() instanceof IFile && (delta.getFlags() & IResourceDelta.CONTENT)!=0){ 
                                File file = delta.getResource().getLocation().toFile();
                                if (file.exists())
                                    printFileInfo(buffer, file);
                            }
                            buffer.append("\n");
                        }
                        return true;
                    }
                });
            }
            catch (CoreException e) {
                Activator.logMessage("exception while logging resource delta: " + e.getClass().getSimpleName() + ": " + e.getMessage());
            }
        }
        Activator.logMessage(buffer.toString());
    }

    protected String eventTypeToString(int eventType) {
        switch (eventType) {
            case IResourceChangeEvent.POST_BUILD: return "POST_BUILD";
            case IResourceChangeEvent.POST_CHANGE: return "POST_CHANGE";
            case IResourceChangeEvent.PRE_BUILD: return "PRE_BUILD";
            case IResourceChangeEvent.PRE_CLOSE: return "PRE_CLOSE";
            case IResourceChangeEvent.PRE_DELETE: return "PRE_DELETE";
            default: return "unknown-type";
        }
    }

    @SuppressWarnings("restriction")
    protected String toString(IResourceDelta delta) {
        // LEGEND: [+] added, [-] removed, [*] changed, [>] and [<] phantom added/removed;
        // then: {CONTENT, MOVED_FROM, MOVED_TO, OPEN, TYPE, SYNC, MARKERS, REPLACED, DESCRIPTION, ENCODING}
        return ((ResourceDelta)delta).toDebugString();
    }

    protected void printFileInfo(final StringBuilder buffer, File file) {
        buffer.append("  length=" + file.length());
        if (logFileContent) {
            try {
                // read at most numFirstBytesToRead
                int length = (int) Math.min(numFirstBytesToLog, file.length());
                DataInputStream in = new DataInputStream(new FileInputStream(file));
                byte[] content = new byte[length];
                in.read(content);
                in.close();
                if (isProbablyText(content)) {
                    buffer.append("\n" + StringUtils.indentLines(new String(content), "    |"));
                    if (length<file.length())
                        buffer.append("    [...]\n");
                }
                else { 
                    buffer.append("  [appears to be binary]");
                }
            }
            catch (IOException e) {
                buffer.append("  [exception reading file content]");
            }
        }
    }

    /** 
     * Some heuristics to determine whether file is text. 
     */
    protected static boolean isProbablyText(byte[] content) {
        int n = Math.min(200, content.length);
        int numAscii = 0;
        for (int i=0; i<n; i++)
            if (content[i] >= '\t')  // byte is signed, so >=128 ones are negative
                numAscii++;
        return n<=5 || numAscii > n/4;
    }

}

