/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ResultFilesTracker;
import org.omnetpp.scave.editors.treeproviders.Sorter;
import org.omnetpp.scave.engine.OrderedKeyValueList;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.InputFile;

public class InputsTree extends TreeViewer {
    private Analysis analysis;
    private ResultFileManager manager;
    private IContainer baseFolder;
    private boolean groupRunFields;

    private Map<InputFile,List<FileNode>> matchingFiles = new HashMap<>();

    class FileNode {
        String filePath;
        List<RunNode> runs = new ArrayList<>();
    }

    class RunNode {
        String runId;
        List<AttrGroup> attrGroups = null;
        List<AttrNode> attrs = null;
    }

    class AttrGroup {
        String name;
        List<AttrNode> attrs;
        AttrGroup(String name, List<AttrNode> attrs) {this.name = name; this.attrs = attrs;}
    }

    enum AttrType { ATTR, ITERVAR, PARAMASSIGNMENT };

    class AttrNode {
        AttrType type;
        String name;
        String value;
        AttrNode(AttrType type, String name, String value) {this.type=type; this.name=name; this.value = value;}
    }

    public InputsTree(Composite parent, Analysis analysis, ResultFileManager manager, IContainer baseFolder, boolean groupRunFields) {
        super(parent, SWT.BORDER | SWT.MULTI);
        this.analysis = analysis;
        this.manager = manager;
        this.baseFolder = baseFolder;
        this.groupRunFields = groupRunFields;
        setContentProvider(new InputsViewContentProvider());
        setLabelProvider(new InputsTreeLabelProvider());
        setInput(analysis);
    }

    public void refresh() {
        rebuildCache();
        super.refresh();
    }

    private void rebuildCache() {
        ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
            @Override
            public Object call() {
                long start = System.currentTimeMillis();
                matchingFiles.clear();
                IPath baseDir = baseFolder.getFullPath();
                ResultFile[] resultFiles = Sorter.sort(manager.getFiles());
                for (InputFile inputFile : analysis.getInputs().getInputs()) {
                    IPath filePatternPath = new Path(inputFile.getName());
                    if (!filePatternPath.isAbsolute())
                        filePatternPath = baseDir.append(filePatternPath);
                    String filePattern = filePatternPath.toString();

                    List<FileNode> fileNodes = new ArrayList<>();
                    matchingFiles.put(inputFile, fileNodes);
                    for (ResultFile resultFile : resultFiles) {
                        if (ResultFilesTracker.matchPattern(resultFile.getFilePath(), filePattern)) {
                            FileNode fileNode = new FileNode();
                            fileNode.filePath = resultFile.getFilePath();
                            fileNodes.add(fileNode);

                            RunList runlist = manager.getRunsInFile(resultFile);
                            for (Run run : Sorter.sort(runlist)) {
                                RunNode runNode = new RunNode();
                                runNode.runId = run.getRunName();
                                fileNode.runs.add(runNode);

                                if (groupRunFields) {
                                    runNode.attrGroups = new ArrayList<>();
                                    runNode.attrGroups.add(new AttrGroup("Iteration Variables", convert(run.getIterationVariables(), AttrType.ITERVAR)));
                                    runNode.attrGroups.add(new AttrGroup("Run Attributes", convert(run.getAttributes(), AttrType.ATTR)));
                                    runNode.attrGroups.add(new AttrGroup("Parameter Assignments", convert(run.getParamAssignments(), AttrType.PARAMASSIGNMENT)));
                                }
                                else {
                                    runNode.attrs = new ArrayList<>();
                                    runNode.attrs.addAll(convert(run.getIterationVariables(), AttrType.ITERVAR));
                                    runNode.attrs.addAll(convert(run.getAttributes(), AttrType.ATTR));
                                    runNode.attrs.addAll(convert(run.getParamAssignments(), AttrType.PARAMASSIGNMENT));
                                }
                            }
                        }
                    }
                }
                Debug.println("InputTree rebuild: " + (System.currentTimeMillis()-start) + "ms");
                return null;
            }

            private List<AttrNode> convert(StringMap attrs, AttrType type) {
                List<AttrNode> result = new ArrayList<>();
                for (String name : attrs.keys().toArray())
                    result.add(new AttrNode(type, name, attrs.get(name)));
                return result;
            }

            private List<AttrNode> convert(OrderedKeyValueList list, AttrType type) {
                List<AttrNode> result = new ArrayList<>();
                for (int i = 0; i < list.size(); i++)
                    result.add(new AttrNode(type, list.get(i).getFirst(), list.get(i).getSecond()));
                return result;
            }

        });
    }

    private class InputsViewContentProvider implements ITreeContentProvider {
        public Object[] getChildren(Object element) {
            if (element instanceof Analysis)
                return analysis.getInputs().getInputs().toArray();
            else if (element instanceof InputFile)
                return matchingFiles.get(element).toArray();
            else if (element instanceof FileNode)
                return ((FileNode)element).runs.toArray();
            else if (element instanceof RunNode)
                return groupRunFields ? ((RunNode) element).attrGroups.toArray() : ((RunNode) element).attrs.toArray();
            else if (element instanceof AttrGroup)
                return ((AttrGroup) element).attrs.toArray();
            else
                return new Object[0];
        }

        public Object getParent(Object element) {
            if (element instanceof InputFile)
                return analysis;
            return null; // setSelection() won't work for other nodes, but we don't need it either
        }

        public boolean hasChildren(Object element) {
            return getChildren(element).length > 0;
        }

        public Object[] getElements(Object inputElement) {
            return getChildren(inputElement);
        }

        public void dispose() {
        }

        public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
        }
    }

    private class InputsTreeLabelProvider extends LabelProvider {

        @Override
        public String getText(Object element) {
            if (element instanceof InputFile)
                return ((InputFile)element).getName() + " (matches " + StringUtils.formatCounted(matchingFiles.get(element).size(), "file") + ")";
            else if (element instanceof FileNode)
                return ((FileNode)element).filePath;
            else if (element instanceof RunNode)
                return StringUtils.defaultIfBlank(((RunNode)element).runId, "(unnamed run)");
            else if (element instanceof AttrGroup)
                return ((AttrGroup)element).name;
            else if (element instanceof AttrNode)
                return ((AttrNode)element).name + " = " + ((AttrNode)element).value;
            return null;
        }

        @Override
        public Image getImage(Object element) {
            if (element instanceof InputFile) {
                boolean empty = matchingFiles.get(element).size() == 0;
                return ScavePlugin.getCachedImage(empty ? ScaveImages.IMG_OBJ16_INPUTFILE_INACTIVE : ScaveImages.IMG_OBJ16_INPUTFILE);
            }
            else if (element instanceof FileNode) {
                String path = ((FileNode)element).filePath;
                if (path.endsWith(".vec"))
                    return ScavePlugin.getCachedImage(ScaveImages.IMG_VECFILE);
                else if (path.endsWith(".sca"))
                    return ScavePlugin.getCachedImage(ScaveImages.IMG_SCAFILE);
                else if (path.endsWith(".vci"))
                    return ScavePlugin.getCachedImage(ScaveImages.IMG_VCIFILE);
                else
                    return ScavePlugin.getCachedImage(ScaveImages.IMG_SCAVEFILE);
            }
            else if (element instanceof RunNode)
                return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_RUN);
            else if (element instanceof AttrGroup)
                return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_FOLDER);
            else if (element instanceof AttrNode) {
                switch (((AttrNode) element).type) {
                    case ATTR: return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_RUNATTR);
                    case ITERVAR: return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_ITERVAR);
                    case PARAMASSIGNMENT: return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_PARAMASSIGNMENT);
                }
            }
            return null;
        }
    }

}
