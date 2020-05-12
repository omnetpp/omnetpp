/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;

import org.eclipse.jface.viewers.ColumnViewerEditorActivationEvent;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerCell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.FileRun;
import org.omnetpp.scave.engine.FileRunList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.OrderedKeyValueList;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.StringPair;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.InputFile;

/**
 * The main tree viewer on the "Inputs" page of the Analysis editor.
 *
 * @author andras
 */
public class InputsTree extends TreeViewer {
    private Analysis analysis;
    private ResultFileManager manager;

    private Map<InputFile,Integer> numFilesPerInput = new HashMap<>();
    private Map<InputFile,List<Object>> inputChildren = new HashMap<>();

    class FileGroupNode {
        String label;
        List<FileNode> files = new ArrayList<>();
        public String toString() {return "Group " + q(label);}
    }

    class FileNode {
        String fileName;
        String filePath;
        List<RunNode> runs = null;
        int numRuns;
        long fileSize = -1;
        public String toString() {return "File " + q(filePath);}
    }

    class RunNode {
        String runId;
        List<AttrGroup> attrGroups = null;
        List<LeafNode> attrs = null;
        int numParameters;
        int numScalars;
        int numVectors;
        int numStatistics;
        int numHistograms;
        public String toString() {return "Run " + q(runId);}
    }

    class AttrGroup {
        String name;
        List<LeafNode> attrs;
        AttrGroup(String name, List<LeafNode> attrs) {this.name = name; this.attrs = attrs;}
        public String toString() {return "Group " + q(name);}
    }

    enum LeafType { ATTR, ITERVAR, CONFIGENTRY, PARAMCONFIGENTRY, RESULT, INFO };

    class LeafNode {
        LeafType type;
        String name;
        String value;
        LeafNode(LeafType type, String name, String value) {this.type=type; this.name=name; this.value = value;}
        public String toString() {return q(name);}
    }

    public InputsTree(Composite parent, Analysis analysis, ResultFileManager manager) {
        super(parent, SWT.BORDER | SWT.MULTI);
        this.analysis = analysis;
        this.manager = manager;
        setContentProvider(new InputsViewContentProvider());
        setLabelProvider(new InputsTreeLabelProvider());
        setInput(analysis);
    }

    private static String q(String s) {
        return "\"" + s + "\"";
    }

    public void refresh() {
        rebuildCache();
        super.refresh();
    }

    private void rebuildCache() {
        Debug.time("Inputs.rebuildCache", 5, () -> {
            ResultFileManager.runWithReadLock(manager, () -> {
                inputChildren.clear();
                numFilesPerInput.clear();
                for (InputFile inputFile : analysis.getInputs().getInputs()) {
                    ResultFileList resultFiles = manager.getFilesForInput(inputFile.getName());
                    List<Object> fileNodes = new ArrayList<>();
                    long numFiles = resultFiles.size();
                    for (int i = 0; i < numFiles; i++) {
                        ResultFile resultFile = resultFiles.get(i);
                        FileNode fileNode = new FileNode();
                        fileNode.fileName = resultFile.getFileName();
                        fileNode.filePath = resultFile.getFilePath();
                        fileNode.numRuns = manager.getNumRunsInFile(resultFile);
                        fileNodes.add(fileNode);
                    }
                    numFilesPerInput.put(inputFile, fileNodes.size());
                    int N = 1000; // group size
                    if (fileNodes.size() <= 2*N) {
                        inputChildren.put(inputFile, fileNodes);
                    }
                    else {
                        List<Object> fileGroupNodes = new ArrayList<>();
                        for (int pos = 0; pos < fileNodes.size(); pos += N) {
                            int end = Math.min(pos+N, fileNodes.size());
                            FileGroupNode fileGroup = new FileGroupNode();
                            fileGroup.label = "[" + pos + ".." + (end-1) + "]";
                            for (int i = pos; i < end; i++)
                                fileGroup.files.add((FileNode)fileNodes.get(i));
                            fileGroupNodes.add(fileGroup);
                        }
                        inputChildren.put(inputFile, fileGroupNodes);
                    }
                }
            });
        });
    }

    /**
     * Content provider
     */
    private class InputsViewContentProvider implements ITreeContentProvider {
        public Object[] getChildren(Object element) {
            return Debug.timed("Inputs.getChildren of "+element, 5, () -> {
                if (element instanceof Analysis)
                    return analysis.getInputs().getInputs().toArray();
                else if (element instanceof InputFile)
                    return inputChildren.get(element).toArray();
                else if (element instanceof FileGroupNode)
                    return ((FileGroupNode) element).files.toArray();
                else if (element instanceof FileNode) {
                    FileNode fileNode = (FileNode)element;
                    if (fileNode.runs == null) {
                        ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
                            @Override
                            public Object call() {
                                fileNode.runs = new ArrayList<>();
                                ResultFile resultFile = manager.getFile(fileNode.filePath);
                                RunList runlist = manager.getRunsInFile(resultFile);
                                for (Run run : runlist.toArray()) {
                                    RunNode runNode = new RunNode();
                                    runNode.runId = run.getRunName();
                                    FileRun fileRun = manager.getFileRun(resultFile, run);
                                    runNode.numParameters = manager.getNumParametersInFileRun(fileRun);
                                    runNode.numScalars = manager.getNumScalarsInFileRun(fileRun);
                                    runNode.numVectors = manager.getNumVectorsInFileRun(fileRun);
                                    runNode.numStatistics = manager.getNumStatisticsInFileRun(fileRun);
                                    runNode.numHistograms = manager.getNumHistogramsInFileRun(fileRun);
                                    fileNode.runs.add(runNode);
                                }
                                return null;
                            }
                        });
                    }
                    return fileNode.runs.toArray();
                }
                else if (element instanceof RunNode) {
                    RunNode runNode = (RunNode)element;
                    if (runNode.attrGroups == null && runNode.attrs == null) {
                        ResultFileManager.runWithReadLock(manager, () -> {
                            Run run = manager.getRunByName(runNode.runId);
                            runNode.attrGroups = new ArrayList<>();
                            addGroup(runNode, "Iteration Variables", makeAttrNodes(run.getIterationVariables(), LeafType.ITERVAR));
                            addGroup(runNode, "Run Attributes", makeAttrNodes(run.getAttributes(), LeafType.ATTR));
                            addGroup(runNode, "Configuration", makeConfigEntryNodes(run));
                            addGroup(runNode, "Saved Parameters", runNode.numParameters, makeFirstResultsNodes(run, ResultFileManager.PARAMETER));
                            addGroup(runNode, "Scalars", runNode.numScalars, makeFirstResultsNodes(run, ResultFileManager.SCALAR));
                            addGroup(runNode, "Statistics", runNode.numStatistics, makeFirstResultsNodes(run, ResultFileManager.STATISTICS));
                            addGroup(runNode, "Histograms", runNode.numHistograms, makeFirstResultsNodes(run, ResultFileManager.HISTOGRAM));
                            addGroup(runNode, "Vectors", runNode.numVectors, makeFirstResultsNodes(run, ResultFileManager.VECTOR));
                        });
                    }
                    return ((RunNode) element).attrGroups.toArray();
                }
                else if (element instanceof AttrGroup)
                    return ((AttrGroup) element).attrs.toArray();
                else
                    return new Object[0];
            });
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

    private void addGroup(RunNode runNode, String label, List<LeafNode> children) {
        addGroup(runNode, label, children == null ? 0 : children.size(), children);
    }

    private void addGroup(RunNode runNode, String label, int numChildren, List<LeafNode> children) {
        if (numChildren > 0)
            runNode.attrGroups.add(new AttrGroup(label+" ("+numChildren+")", children));
    }

    private List<LeafNode> makeAttrNodes(StringMap attrs, LeafType type) {
        List<LeafNode> result = new ArrayList<>();
        for (String name : attrs.keys().toArray())
            result.add(new LeafNode(type, name, attrs.get(name)));
        return result;
    }

    private List<LeafNode> makeConfigEntryNodes(Run run) {
        OrderedKeyValueList configEntries = run.getConfigEntries();
        List<LeafNode> result = new ArrayList<>();
        for (int i = 0; i < configEntries.size(); i++) {
            StringPair keyValue = configEntries.get(i);
            LeafType type = Run.isParamAssignmentConfigKey(keyValue.getFirst()) ? LeafType.PARAMCONFIGENTRY : LeafType.CONFIGENTRY;
            result.add(new LeafNode(type, keyValue.getFirst(), keyValue.getSecond()));
        }
        return result;
    }

    private List<LeafNode> makeFirstResultsNodes(Run run, int type) {
        List<LeafNode> nodes = new ArrayList<>();
        FileRunList fileRuns = run.getFileRuns();
        IDList itemIDs = manager.getItems(fileRuns, type, false);
        int count = itemIDs.size();
        if (count > 10)
            itemIDs = itemIDs.getRange(0, 9);
        for (int i = 0; i < itemIDs.size(); i++) {
            ResultItem item = manager.getItem(itemIDs.get(i));
            String value = null; // or String.valueOf(item.getValue())) or something
            nodes.add(new LeafNode(LeafType.RESULT, item.getModuleName() + "." + item.getName(), value));
        }
        if (itemIDs.size() < count)
            nodes.add(new LeafNode(LeafType.ATTR, "... and " + (count-itemIDs.size()) + " more", null));
        return nodes;
    }


    /**
     * Label provider
     */
    private class InputsTreeLabelProvider extends LabelProvider {

        @Override
        public String getText(Object element) {
            return Debug.timed("Inputs.getText of "+element, 5, () -> {
                if (element instanceof InputFile)
                    return ((InputFile)element).getName() + "  (matches " + StringUtils.formatCounted(numFilesPerInput.get(element), "file") + ")";
                else if (element instanceof FileGroupNode)
                    return ((FileGroupNode) element).label;
                else if (element instanceof FileNode) {
                    FileNode fileNode = (FileNode)element;
                    if (fileNode.fileSize == -1)
                        fileNode.fileSize = new File(getFileSystemFilePath(fileNode)).length();
                    return fileNode.filePath + "  (" + FileUtils.humanReadableByteCountBin(fileNode.fileSize) + ", " + StringUtils.formatCounted(fileNode.numRuns, "run") +  ")";
                }
                else if (element instanceof RunNode) {
                    RunNode runNode = (RunNode)element;
                    return StringUtils.defaultIfBlank(runNode.runId, "(unnamed run)") + "  (" + getContentSummary(runNode) + ")";
                }
                else if (element instanceof AttrGroup)
                    return ((AttrGroup)element).name;
                else if (element instanceof LeafNode) {
                    LeafNode leafNode = (LeafNode)element;
                    if (leafNode.value == null)
                        return leafNode.name;
                    else
                        return leafNode.name + " = " + leafNode.value;
                }
                return null;
            });
        }

        private String getFileSystemFilePath(FileNode fileNode) {
            String fileSystemFilePath = ResultFileManager.callWithReadLock(manager, new Callable<String>() {
                @Override
                public String call() {
                     return manager.getFile(fileNode.filePath).getFileSystemFilePath();
                }
            });
            return fileSystemFilePath;
        }

        private String getContentSummary(RunNode runNode) {
            StringBuffer buffer = new StringBuffer();
            if (runNode.numParameters > 0)
                buffer.append(", " + StringUtils.formatCounted(runNode.numParameters, "saved parameter"));
            if (runNode.numScalars > 0)
                buffer.append(", " + StringUtils.formatCounted(runNode.numScalars, "scalar"));
            if (runNode.numVectors > 0)
                buffer.append(", " + StringUtils.formatCounted(runNode.numVectors, "vector"));
            if (runNode.numStatistics > 0)
                buffer.append(", " + StringUtils.formatCounted(runNode.numStatistics, "statistic"));
            if (runNode.numHistograms > 0)
                buffer.append(", " + StringUtils.formatCounted(runNode.numHistograms, "histogram"));
            return buffer.toString().substring(2);
        }

        @Override
        public Image getImage(Object element) {
            if (element instanceof InputFile) {
                boolean empty = inputChildren.get(element).size() == 0;
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
            else if (element instanceof LeafNode) {
                switch (((LeafNode) element).type) {
                    case ATTR: return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_RUNATTR);
                    case ITERVAR: return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_ITERVAR);
                    case CONFIGENTRY: return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_CONFIGENTRY);
                    case PARAMCONFIGENTRY: return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_PARAMASSIGNMENT);
                    case RESULT: return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_PARAMASSIGNMENT); //TODO
                    case INFO: return null;
                }
            }
            return null;
        }
    }

    protected void hookEditingSupport(Control control) {
        control.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.keyCode == SWT.F2 && getTree().getSelectionCount() == 1) {
                    ViewerCell cell = getViewerRowFromItem(getTree().getSelection()[0]).getCell(0);
                    if (cell != null)
                        triggerEditorActivationEvent(new ColumnViewerEditorActivationEvent(cell));
                }
            }
        });
    }
}
