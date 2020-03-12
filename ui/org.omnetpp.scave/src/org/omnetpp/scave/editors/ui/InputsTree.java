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
import org.omnetpp.scave.editors.treeproviders.Sorter;
import org.omnetpp.scave.engine.FileRun;
import org.omnetpp.scave.engine.OrderedKeyValueList;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.InputFile;

public class InputsTree extends TreeViewer {
    private Analysis analysis;
    private ResultFileManager manager;
    private boolean groupRunFields;

    private Map<InputFile,Integer> numFilesPerInput = new HashMap<>();
    private Map<InputFile,List<Object>> inputChildren = new HashMap<>();

    class FileGroupNode {
        String label;
        List<FileNode> files = new ArrayList<>();
    }

    class FileNode {
        String fileName;
        String filePath;
        List<RunNode> runs = null;
        int numRuns;
        long fileSize = -1;
    }

    class RunNode {
        String runId;
        List<AttrGroup> attrGroups = null;
        List<AttrNode> attrs = null;
        int numParameters;
        int numScalars;
        int numVectors;
        int numStatistics;
        int numHistograms;
    }

    class AttrGroup {
        String name;
        List<AttrNode> attrs;
        AttrGroup(String name, List<AttrNode> attrs) {this.name = name; this.attrs = attrs;}
    }

    enum AttrType { ATTR, ITERVAR, PARAMASSIGNMENT, CONFIGENTRY };

    class AttrNode {
        AttrType type;
        String name;
        String value;
        AttrNode(AttrType type, String name, String value) {this.type=type; this.name=name; this.value = value;}
    }

    public InputsTree(Composite parent, Analysis analysis, ResultFileManager manager, boolean groupRunFields) {
        super(parent, SWT.BORDER | SWT.MULTI);
        this.analysis = analysis;
        this.manager = manager;
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
        ResultFileManager.runWithReadLock(manager, () -> {
            long start = System.currentTimeMillis();
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
            long end = System.currentTimeMillis();
            Debug.println("InputsTree: build of 2nd tree level (file lists) took: " + (end-start) + "ms");
        });
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

    /**
     * Content provider
     */
    private class InputsViewContentProvider implements ITreeContentProvider {
        public Object[] getChildren(Object element) {
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
                            for (Run run : Sorter.sort(runlist)) {
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
                    ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
                        @Override
                        public Object call() {
                            Run run = manager.getRunByName(runNode.runId);
                            if (groupRunFields) {
                                runNode.attrGroups = new ArrayList<>();
                                runNode.attrGroups.add(new AttrGroup("Iteration Variables", convert(run.getIterationVariables(), AttrType.ITERVAR)));
                                runNode.attrGroups.add(new AttrGroup("Run Attributes", convert(run.getAttributes(), AttrType.ATTR)));
                                runNode.attrGroups.add(new AttrGroup("Parameter Assignments", convert(run.getParamAssignments(), AttrType.PARAMASSIGNMENT)));
                                runNode.attrGroups.add(new AttrGroup("Configuration", convert(run.getNonParamAssignmentConfigEntries(), AttrType.CONFIGENTRY)));
                            }
                            else {
                                runNode.attrs = new ArrayList<>();
                                runNode.attrs.addAll(convert(run.getIterationVariables(), AttrType.ITERVAR));
                                runNode.attrs.addAll(convert(run.getAttributes(), AttrType.ATTR));
                                runNode.attrs.addAll(convert(run.getParamAssignments(), AttrType.PARAMASSIGNMENT));
                                runNode.attrs.addAll(convert(run.getNonParamAssignmentConfigEntries(), AttrType.CONFIGENTRY));
                            }
                            return null;
                        }
                    });
                }
                return groupRunFields ? ((RunNode) element).attrGroups.toArray() : ((RunNode) element).attrs.toArray();
            }
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

    /**
     * Label provider
     */
    private class InputsTreeLabelProvider extends LabelProvider {

        @Override
        public String getText(Object element) {
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
            else if (element instanceof AttrNode)
                return ((AttrNode)element).name + " = " + ((AttrNode)element).value;
            return null;
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
            else if (element instanceof AttrNode) {
                switch (((AttrNode) element).type) {
                    case ATTR: return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_RUNATTR);
                    case ITERVAR: return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_ITERVAR);
                    case PARAMASSIGNMENT: return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_PARAMASSIGNMENT);
                    case CONFIGENTRY: return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_CONFIGENTRY);
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
