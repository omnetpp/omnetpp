package freemarker.eclipse.debug;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.debug.ui.console.FileLink;
import org.eclipse.debug.ui.console.IConsole;
import org.eclipse.debug.ui.console.IConsoleLineTracker;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IRegion;

/**
 * @version $Id: FreemarkerConsoleLineTracker.java,v 1.1 2005/01/25 18:59:29 stephanmueller Exp $
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller </a>
 */
public class FreemarkerConsoleLineTracker implements IConsoleLineTracker {

    private IConsole console;
    private static final Pattern pattern = Pattern.compile("^.*\\[on line (\\d+), column (\\d+) in (\\S+)\\]$");

    /*
     * (non-Javadoc)
     *
     * @see org.eclipse.debug.ui.console.IConsoleLineTracker#init(org.eclipse.debug.ui.console.IConsole)
     */
    public void init(IConsole console) {
        this.console = console;
    }

    /*
     * (non-Javadoc)
     *
     * @see org.eclipse.debug.ui.console.IConsoleLineTracker#lineAppended(org.eclipse.jface.text.IRegion)
     */
    @SuppressWarnings("deprecation")
    public void lineAppended(IRegion line) {
        try {
            String text = console.getDocument().get(line.getOffset(), line.getLength());
            Matcher m = pattern.matcher(text);
            if (m.matches()) {
                String res = m.group(3);
                int l = Integer.parseInt(m.group(1));
                //int c = Integer.parseInt(m.group(2));

                int linkOffset = line.getOffset() + m.start(1) - 9;
                int linkLength = line.getLength() - m.start(1) + 9;

                IProject[] projects = ResourcesPlugin.getWorkspace().getRoot().getProjects();

                List<IFile> files = new ArrayList<IFile>();
                for (int i = 0; i < projects.length; i++) {
                    IProject project = projects[i];
//commenting out a few lines, to remove dependency on JDT  --Andras
//                  IJavaProject jproject = JavaCore.create(project);

                    IFile[] foundFiles = retrieveFiles(project, res);

                    for (int j = 0; j < foundFiles.length; j++) {
                        IFile file = foundFiles[j];

                        boolean isInsideOutputLocation = false;

//more lines commented out --Andras
//                      try {
//                          // check whether the file is located inside a Java project's output location
//                          isInsideOutputLocation = jproject.getOutputLocation().isPrefixOf(file.getFullPath());
//                      } catch (JavaModelException e1) {
//                          // noop; isInsideOutputLocation = false
//                      }

                        // same check as above, without pulling in dependence on JDT --Andras
                        isInsideOutputLocation = file.getFullPath().segment(1).equals("bin");

                        if(!isInsideOutputLocation) {
                            // FTL files inside a project's output location are probably copies
                            files.add(file);
                        }
                    }
                }

                if (files.size() != 0) {
                    List<IFile> filteredFiles = new ArrayList<IFile>();

                    for (int j = 0; j < files.size(); j++) {
                        IFile aFile = (IFile) files.get(j);
                        IContainer parent = aFile.getParent();
                        while (parent != null) {
                            parent = parent.getParent();
                        }
                        filteredFiles.add(aFile);
                    }

                    if (filteredFiles.size() != 0) {
                        IFile file = (IFile) filteredFiles.get(0);
                        if (file != null && file.exists()) {
                            FileLink link = new FileLink(file, null, -1, -1, l);
                            console.addLink(link, linkOffset, linkLength);
                        }
                    }
                }
            }
        } catch (BadLocationException e) {
        }
    }

    public IFile[] retrieveFiles(IContainer container, String filename) {
        String[] seqs = filename.split("/|\\\\");

        List<IContainer> l = new LinkedList<IContainer>();
        l.add(container);
        List<IFile> files = new ArrayList<IFile>();
        while (!l.isEmpty()) {
            IContainer c = (IContainer) l.get(0);
            l.remove(0);
            try {
                IResource[] res = c.members();
                for (int i = 0; i < res.length; i++) {
                    IResource resource = res[i];
                    if (resource instanceof IContainer) {
                        l.add((IContainer)resource);
                    } else if (resource instanceof IFile) {
                        if (isCorrectFile((IFile) resource, seqs)) {
                            files.add((IFile)resource);
                        }
                    }
                }
            } catch (CoreException ce) {
            }
        }
        IFile[] f = new IFile[files.size()];
        files.toArray(f);
        return f;
    }

    private boolean isCorrectFile(IFile file, String[] filenameSeqs) {
        IResource temp = file;
        for (int i = filenameSeqs.length - 1; i >= 0; i--) {
            String seq = filenameSeqs[i];
            if (!seq.equals(temp.getName())) {
                return false;
            }
            temp = temp.getParent();
        }
        return true;
    }

    /*
     * (non-Javadoc)
     *
     * @see org.eclipse.debug.ui.console.IConsoleLineTracker#dispose()
     */
    public void dispose() {
        // do nothing here
    }

}