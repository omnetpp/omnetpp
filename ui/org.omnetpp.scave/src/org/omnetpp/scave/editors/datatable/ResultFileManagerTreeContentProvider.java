package org.omnetpp.scave.editors.datatable;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.Set;
import java.util.concurrent.Callable;

import org.apache.commons.collections.map.MultiValueMap;
import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.CollectionUtils;
import org.omnetpp.scave.engine.DoubleVector;
import org.omnetpp.scave.engine.FileRun;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunAttribute;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.VectorResult;

/**
 * This class provides a customizable tree of various data from the result file manager.
 * The tree is built up from levels that may be freely reordered and switched on/off.
 * Levels include experiment, measurement, replication, config, run number, file name, run id, module path, module name, result item, result item attributes, etc.
 *
 * @author levy
 */
public class ResultFileManagerTreeContentProvider {
    protected ResultFileManager manager;

    protected IDList idList;

    protected Class<? extends Node>[] levels;

    public ResultFileManagerTreeContentProvider() {
        setDefaultLevels();
    }

    public void setResultFileManager(ResultFileManager manager) {
        this.manager = manager;
    }

    public void setIDList(IDList idList) {
        this.idList = idList;
    }

    public Class<? extends Node>[] getLevels() {
        return levels;
    }

    public void setLevels(Class<? extends Node>[] levels) {
        this.levels = levels;
    }

    @SuppressWarnings("unchecked")
    public void setDefaultLevels() {
        levels = new Class[4];
        levels[0] = ExperimentMeasurementReplicationNode.class;
        levels[1] = ModulePathNode.class;
        levels[2] = ResultItemNode.class;
        levels[3] = ResultItemAttributeNode.class;
    }

    public List<Node> getChildNodes(final List<Node> path) {
        final Node firstNode = path.size() == 0 ? null : path.get(0);

        return ResultFileManager.callWithReadLock(manager, new Callable<List<Node>>() {
            @SuppressWarnings("unchecked")
            public List<Node> call() throws Exception {
                MultiValueMap nodeIdsMap = new MultiValueMap();
                int currentLevelIndex;
                if (firstNode == null)
                    currentLevelIndex = -1;
                else {
                    currentLevelIndex = ArrayUtils.indexOf(levels, firstNode.getClass());
                    if (currentLevelIndex == -1)
                        return new ArrayList<Node>();
                }
                int nextLevelIndex;
                if (firstNode instanceof ModuleNameNode) {
                    ModuleNameNode moduleNameNode = (ModuleNameNode)firstNode;
                    nextLevelIndex = currentLevelIndex + (moduleNameNode.leaf ? 1 : 0);
                }
                else
                    nextLevelIndex = currentLevelIndex + 1;
                Class<? extends Node> nextLevelClass = nextLevelIndex < levels.length ? levels[nextLevelIndex] : null;
                if (nextLevelClass != null) {
                    int idCount = firstNode == null ? idList.size() : firstNode.ids.length;
                    for (int i = 0; i < idCount; i++) {
                        long id = firstNode == null ? idList.get(i) : firstNode.ids[i];
                        ResultItem resultItem = manager.getItem(id);
                        FileRun fileRun = resultItem.getFileRun();
                        Run run = fileRun.getRun();
                        if (matchesPath(path, id, resultItem)) {
                            if (nextLevelClass.equals(ExperimentNode.class))
                                nodeIdsMap.put(new ExperimentNode(run.getAttribute(RunAttribute.EXPERIMENT)), id);
                            else if (nextLevelClass.equals(MeasurementNode.class))
                                nodeIdsMap.put(new MeasurementNode(run.getAttribute(RunAttribute.MEASUREMENT)), id);
                            else if (nextLevelClass.equals(ReplicationNode.class))
                                nodeIdsMap.put(new ReplicationNode(run.getAttribute(RunAttribute.REPLICATION)), id);
                            else if (nextLevelClass.equals(ExperimentMeasurementReplicationNode.class))
                                nodeIdsMap.put(new ExperimentMeasurementReplicationNode(run.getAttribute(RunAttribute.EXPERIMENT), run.getAttribute(RunAttribute.MEASUREMENT), run.getAttribute(RunAttribute.REPLICATION)), id);
                            else if (nextLevelClass.equals(ConfigNode.class))
                                nodeIdsMap.put(new ConfigNode(run.getAttribute(RunAttribute.CONFIGNAME)), id);
                            else if (nextLevelClass.equals(RunNumberNode.class))
                                nodeIdsMap.put(new RunNumberNode(run.getRunNumber()), id);
                            else if (nextLevelClass.equals(ConfigRunNumberNode.class))
                                nodeIdsMap.put(new ConfigRunNumberNode(run.getAttribute(RunAttribute.CONFIGNAME), run.getRunNumber()), id);
                            else if (nextLevelClass.equals(FileNameNode.class))
                                nodeIdsMap.put(new FileNameNode(fileRun.getFile().getFileName()), id);
                            else if (nextLevelClass.equals(RunIdNode.class))
                                nodeIdsMap.put(new RunIdNode(run.getRunName()), id);
                            else if (nextLevelClass.equals(FileNameRunIdNode.class))
                                nodeIdsMap.put(new FileNameRunIdNode(fileRun.getFile().getFileName(), run.getRunName()), id);
                            else if (nextLevelClass.equals(ModuleNameNode.class)) {
                                String moduleName = resultItem.getModuleName();
                                String modulePrefix = getModulePrefix(path, null);
                                if (moduleName.startsWith(modulePrefix)) {
                                    String remainingName = StringUtils.removeStart(StringUtils.removeStart(moduleName, modulePrefix), ".");
                                    String name = StringUtils.substringBefore(remainingName, ".");
                                    nodeIdsMap.put(new ModuleNameNode(StringUtils.isEmpty(name) ? "." : name, !remainingName.contains(".")), id);
                                }
                            }
                            else if (nextLevelClass.equals(ModulePathNode.class))
                                nodeIdsMap.put(new ModulePathNode(resultItem.getModuleName()), id);
                            else if (nextLevelClass.equals(ResultItemNode.class))
                                nodeIdsMap.put(new ResultItemNode(id), id);
                            else if (nextLevelClass.equals(ResultItemNameNode.class))
                                nodeIdsMap.put(new ResultItemNameNode(resultItem.getName()), id);
                            else if (nextLevelClass.equals(ResultItemAttributeNode.class)) {
                                nodeIdsMap.put(new ResultItemAttributeNode("Module name", String.valueOf(resultItem.getModuleName())), id);
                                nodeIdsMap.put(new ResultItemAttributeNode("Type", resultItem.getType().toString().replaceAll("TYPE_", "").toLowerCase()), id);

                                if (resultItem instanceof ScalarResult) {
                                    ScalarResult scalar = (ScalarResult)resultItem;
                                    nodeIdsMap.put(new ResultItemAttributeNode("Value", String.valueOf(scalar.getValue())), id);
                                }
                                else if (resultItem instanceof VectorResult) {
                                    VectorResult vector = (VectorResult)resultItem;
                                    nodeIdsMap.put(new ResultItemAttributeNode("Count", String.valueOf(vector.getCount())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Min", String.valueOf(vector.getMin())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Max", String.valueOf(vector.getMax())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Mean", String.valueOf(vector.getMean())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("StdDev", String.valueOf(vector.getStddev())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Variance", String.valueOf(vector.getVariance())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Start event number", String.valueOf(vector.getStartEventNum())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("End event number", String.valueOf(vector.getEndEventNum())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Start time", String.valueOf(vector.getStartTime())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("End time", String.valueOf(vector.getEndTime())), id);
                                }
                                else if (resultItem instanceof HistogramResult) {
                                    HistogramResult histogram = (HistogramResult)resultItem;
                                    nodeIdsMap.put(new ResultItemAttributeNode("Count", String.valueOf(histogram.getCount())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Min", String.valueOf(histogram.getMin())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Max", String.valueOf(histogram.getMax())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Mean", String.valueOf(histogram.getMean())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("StdDev", String.valueOf(histogram.getStddev())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Variance", String.valueOf(histogram.getVariance())), id);
                                    DoubleVector bins = histogram.getBins();
                                    DoubleVector values = histogram.getValues();
                                    if (bins != null && values != null && bins.size() > 0 && values.size() > 0) {
                                        ResultItemAttributeNode binsNode = new ResultItemAttributeNode("Bins", String.valueOf(histogram.getBins().size()));
                                        List<Node> list = new ArrayList<Node>();
                                        for (int j = 0; j < bins.size(); j++) {
                                            double bin1 = bins.get(j);
                                            double bin2 = j < bins.size() - 1 ? bins.get(j + 1) : Double.POSITIVE_INFINITY;
                                            double value = values.get(j);
                                            double valueFloor = Math.floor(value);
                                            list.add(new NameValueNode(String.valueOf(bin1) + " .. " + String.valueOf(bin2), value == valueFloor ? String.valueOf((long)valueFloor) : String.valueOf(value)));
                                        }
                                        binsNode.children = list.toArray(new Node[0]);
                                        nodeIdsMap.put(binsNode, id);
                                    }
                                }
                                else
                                    throw new IllegalArgumentException();
                            }
                            else
                                throw new IllegalArgumentException();
                        }
                    }
                }
                else if (firstNode instanceof Node) {
                    Node node = firstNode;
                    if (node.children != null)
                        return Arrays.asList(node.children);
                }

                Set<Node> nodes = nodeIdsMap.keySet();
                List<Node> result = CollectionUtils.toSorted(new ArrayList<Node>(nodes), new Comparator<Node>() {
                    public int compare(Node o1, Node o2) {
                        return (o1).getColumnText(0).compareTo((o2).getColumnText(0));
                    }
                });

                for (Node node : nodes)
                    node.ids = (Long[])nodeIdsMap.getCollection(node).toArray(new Long[0]);

                return result;
            }
        });
    }

    protected static String getModulePrefix(final List<Node> path, Node nodeLimit) {
        StringBuffer modulePrefix = new StringBuffer();
        for (int i =  path.size() - 1; i >= 0; i--) {
            Node node = path.get(i);
            if (node == nodeLimit)
                break;
            String name = null;
            if (node instanceof ModuleNameNode)
                name = ((ModuleNameNode)node).name;
            else if (node instanceof ModulePathNode)
                name = ((ModulePathNode)node).path;
            if (name != null) {
                if (modulePrefix.length() == 0)
                    modulePrefix.append(name);
                else {
                    modulePrefix.append('.');
                    modulePrefix.append(name);
                }
            }
        }
        return modulePrefix.toString();
    }

    protected boolean matchesPath(List<Node> path, long id, ResultItem resultItem) {
        for (Node node : path)
            if (!node.matches(path, id, resultItem))
                return false;
        return true;
    }

    /* Various tree node types */

    @SuppressWarnings("unchecked")
    public static Class[] getAvailableLevelClasses() {
        return new Class[] {
            ExperimentNode.class,
            MeasurementNode.class,
            ReplicationNode.class,
            ExperimentMeasurementReplicationNode.class,
            ConfigNode.class,
            RunNumberNode.class,
            ConfigRunNumberNode.class,
            FileNameNode.class,
            RunIdNode.class,
            FileNameRunIdNode.class,
            ModulePathNode.class,
            ModuleNameNode.class,
            ResultItemNode.class,
            ResultItemNameNode.class,
            ResultItemAttributeNode.class};
    }

    protected static abstract class Node {
        public Long[] ids;

        public Node[] children;

        public boolean isExpandedByDefault() {
            return false;
        }

        public Image getImage() {
            return ImageFactory.getIconImage(ImageFactory.MODEL_IMAGE_FOLDER);
        }

        public abstract String getColumnText(int index);

        public abstract boolean matches(List<Node> path, long id, ResultItem resultItem);
    }

    protected static class NameValueNode extends Node {
        public String name;
        public String value;

        public NameValueNode(String name, String value) {
            this.name = name;
            this.value = value;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? name : value;
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            return true;
        }

        @Override
        public Image getImage() {
            return ImageFactory.getIconImage(ImageFactory.TOOLBAR_IMAGE_PROPERTIES);
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((name == null) ? 0 : name.hashCode());
            result = prime * result + ((value == null) ? 0 : value.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            NameValueNode other = (NameValueNode) obj;
            if (name == null) {
                if (other.name != null)
                    return false;
            }
            else if (!name.equals(other.name))
                return false;
            if (value == null) {
                if (other.value != null)
                    return false;
            }
            else if (!value.equals(other.value))
                return false;
            return true;
        }
    }

    protected static class ExperimentNode extends Node {
        public String name;

        public ExperimentNode(String name) {
            this.name = name;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? name + " (experiment)" : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            Run run = resultItem.getFileRun().getRun();
            return name.equals(run.getAttribute(RunAttribute.EXPERIMENT));
        }

        @Override
        public boolean isExpandedByDefault() {
            return true;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((name == null) ? 0 : name.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            ExperimentNode other = (ExperimentNode) obj;
            if (name == null) {
                if (other.name != null)
                    return false;
            }
            else if (!name.equals(other.name))
                return false;
            return true;
        }
    }

    protected static class MeasurementNode extends Node {
        public String name;

        public MeasurementNode(String name) {
            this.name = name;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? StringUtils.defaultIfEmpty(name, "default")  + " (measurement)" : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            Run run = resultItem.getFileRun().getRun();
            return name.equals(run.getAttribute(RunAttribute.MEASUREMENT));
        }

        @Override
        public boolean isExpandedByDefault() {
            return true;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((name == null) ? 0 : name.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            MeasurementNode other = (MeasurementNode) obj;
            if (name == null) {
                if (other.name != null)
                    return false;
            }
            else if (!name.equals(other.name))
                return false;
            return true;
        }
    }

    protected static class ReplicationNode extends Node {
        public String name;

        public ReplicationNode(String name) {
            this.name = name;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? name + " (replication)" : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            Run run = resultItem.getFileRun().getRun();
            return name.equals(run.getAttribute(RunAttribute.REPLICATION));
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((name == null) ? 0 : name.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            ReplicationNode other = (ReplicationNode) obj;
            if (name == null) {
                if (other.name != null)
                    return false;
            }
            else if (!name.equals(other.name))
                return false;
            return true;
        }
    }

    protected static class ExperimentMeasurementReplicationNode extends Node {
        public String experiment;

        public String measurement;

        public String replication;

        public ExperimentMeasurementReplicationNode(String experiment, String measurement, String replication) {
            this.experiment = experiment;
            this.measurement = measurement;
            this.replication = replication;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? experiment + (StringUtils.isEmpty(measurement) ? "" : " : " + measurement) + " : " + replication : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            Run run = resultItem.getFileRun().getRun();
            return experiment.equals(run.getAttribute(RunAttribute.EXPERIMENT)) && measurement.equals(run.getAttribute(RunAttribute.MEASUREMENT)) && replication.equals(run.getAttribute(RunAttribute.REPLICATION));
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((experiment == null) ? 0 : experiment.hashCode());
            result = prime * result + ((measurement == null) ? 0 : measurement.hashCode());
            result = prime * result + ((replication == null) ? 0 : replication.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            ExperimentMeasurementReplicationNode other = (ExperimentMeasurementReplicationNode) obj;
            if (experiment == null) {
                if (other.experiment != null)
                    return false;
            }
            else if (!experiment.equals(other.experiment))
                return false;
            if (measurement == null) {
                if (other.measurement != null)
                    return false;
            }
            else if (!measurement.equals(other.measurement))
                return false;
            if (replication == null) {
                if (other.replication != null)
                    return false;
            }
            else if (!replication.equals(other.replication))
                return false;
            return true;
        }
    }

    protected static class ConfigNode extends Node {
        public String name;

        public ConfigNode(String name) {
            this.name = name;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? name + " (config)" : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            return resultItem.getFileRun().getRun().getAttribute(RunAttribute.CONFIGNAME).equals(name);
        }

        @Override
        public boolean isExpandedByDefault() {
            return true;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((name == null) ? 0 : name.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            ConfigNode other = (ConfigNode) obj;
            if (name == null) {
                if (other.name != null)
                    return false;
            }
            else if (!name.equals(other.name))
                return false;
            return true;
        }
    }

    protected static class RunNumberNode extends Node {
        public long value;

        public RunNumberNode(long value) {
            this.value = value;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? String.valueOf(value) + " (run number)" : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            return resultItem.getFileRun().getRun().getRunNumber() == value;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + (int) (value ^ (value >>> 32));
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            RunNumberNode other = (RunNumberNode) obj;
            if (value != other.value)
                return false;
            return true;
        }
    }

    protected static class ConfigRunNumberNode extends Node {
        public String config;

        public long runNumber;

        public ConfigRunNumberNode(String config, long runNumber) {
            this.config = config;
            this.runNumber = runNumber;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? config + " : " + runNumber : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            Run run = resultItem.getFileRun().getRun();
            return run.getAttribute(RunAttribute.CONFIGNAME).equals(config) && run.getRunNumber() == runNumber;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((config == null) ? 0 : config.hashCode());
            result = prime * result + (int) (runNumber ^ (runNumber >>> 32));
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            ConfigRunNumberNode other = (ConfigRunNumberNode) obj;
            if (config == null) {
                if (other.config != null)
                    return false;
            }
            else if (!config.equals(other.config))
                return false;
            if (runNumber != other.runNumber)
                return false;
            return true;
        }
    }

    protected static class FileNameNode extends Node {
        public String name;

        public FileNameNode(String name) {
            this.name = name;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? name + " (file name)" : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            return resultItem.getFileRun().getFile().getFileName().equals(name);
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((name == null) ? 0 : name.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            FileNameNode other = (FileNameNode) obj;
            if (name == null) {
                if (other.name != null)
                    return false;
            }
            else if (!name.equals(other.name))
                return false;
            return true;
        }
    }

    protected static class RunIdNode extends Node {
        public String value;

        public RunIdNode(String value) {
            this.value = value;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? value + " (run id)" : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            return resultItem.getFileRun().getRun().getRunName().equals(value);
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((value == null) ? 0 : value.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            RunIdNode other = (RunIdNode) obj;
            if (value == null) {
                if (other.value != null)
                    return false;
            }
            else if (!value.equals(other.value))
                return false;
            return true;
        }
    }

    protected static class FileNameRunIdNode extends Node {
        public String fileName;

        public String runId;

        public FileNameRunIdNode(String fileName, String runId) {
            this.fileName = fileName;
            this.runId = runId;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? fileName + " : " + runId : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            FileRun fileRun = resultItem.getFileRun();
            return fileRun.getFile().getFileName().equals(fileName) && fileRun.getRun().getRunName().equals(runId);
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((fileName == null) ? 0 : fileName.hashCode());
            result = prime * result + ((runId == null) ? 0 : runId.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            FileNameRunIdNode other = (FileNameRunIdNode) obj;
            if (fileName == null) {
                if (other.fileName != null)
                    return false;
            }
            else if (!fileName.equals(other.fileName))
                return false;
            if (runId == null) {
                if (other.runId != null)
                    return false;
            }
            else if (!runId.equals(other.runId))
                return false;
            return true;
        }
    }

    protected static class ModulePathNode extends Node {
        public String path;

        public ModulePathNode(String path) {
            this.path = path;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? path : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            return resultItem.getModuleName().equals(this.path);
        }

        @Override
        public Image getImage() {
            return ImageFactory.getIconImage(ImageFactory.MODEL_IMAGE_SIMPLEMODULE);
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((path == null) ? 0 : path.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            ModulePathNode other = (ModulePathNode) obj;
            if (path == null) {
                if (other.path != null)
                    return false;
            }
            else if (!path.equals(other.path))
                return false;
            return true;
        }
    }

    protected static class ModuleNameNode extends Node {
        public String name;

        public boolean leaf;

        public ModuleNameNode(String name, boolean leaf) {
            this.name = name;
            this.leaf = leaf;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? name : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            String modulePrefix = getModulePrefix(path, this);
            modulePrefix = StringUtils.isEmpty(modulePrefix) ? name : modulePrefix + "." + name;
            return resultItem.getModuleName().startsWith(modulePrefix);
        }

        @Override
        public Image getImage() {
            return ImageFactory.getIconImage(ImageFactory.MODEL_IMAGE_SIMPLEMODULE);
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((name == null) ? 0 : name.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            ModuleNameNode other = (ModuleNameNode) obj;
            if (name == null) {
                if (other.name != null)
                    return false;
            }
            else if (!name.equals(other.name))
                return false;
            return true;
        }
    }

    protected class ResultItemNode extends Node {
        public long id;

        public ResultItemNode(long id) {
            this.id = id;
        }

        @Override
        public String getColumnText(int index) {
            ResultItem resultItem = manager.getItem(id);
            String value = "";
            if (resultItem instanceof ScalarResult) {
                ScalarResult scalar = (ScalarResult)resultItem;
                value = String.valueOf(scalar.getValue());
            }
            else if (resultItem instanceof VectorResult) {
                VectorResult vector = (VectorResult)resultItem;
                value = String.valueOf(vector.getMean()) + " (" + String.valueOf(vector.getCount()) + ")";
            }
            else if (resultItem instanceof HistogramResult) {
                HistogramResult histogram = (HistogramResult)resultItem;
                value = String.valueOf(histogram.getMean()) + " (" + String.valueOf(histogram.getCount()) + ")";
            }
            return index == 0 ? resultItem.getName() + " (" + resultItem.getClass().getSimpleName().replaceAll("Result", "").toLowerCase() + ")" : value;
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            return this.id == id;
        }

        @Override
        public Image getImage() {
            ResultItem resultItem = manager.getItem(id);
            if (resultItem instanceof ScalarResult)
                return ImageFactory.getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWSCALARS);
            else if (resultItem instanceof VectorResult)
                return ImageFactory.getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWVECTORS);
            else if (resultItem instanceof HistogramResult)
                return ImageFactory.getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWHISTOGRAMS);
            else
                return ImageFactory.getIconImage(ImageFactory.DEFAULT);
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + getOuterType().hashCode();
            result = prime * result + (int) (id ^ (id >>> 32));
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            ResultItemNode other = (ResultItemNode) obj;
            if (!getOuterType().equals(other.getOuterType()))
                return false;
            if (id != other.id)
                return false;
            return true;
        }

        private ResultFileManagerTreeContentProvider getOuterType() {
            return ResultFileManagerTreeContentProvider.this;
        }
    }

    protected static class ResultItemNameNode extends Node {
        public String name;

        public ResultItemNameNode(String name) {
            this.name = name;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? name : "";
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            return resultItem.getName().equals(name);
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((name == null) ? 0 : name.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            ResultItemNameNode other = (ResultItemNameNode) obj;
            if (name == null) {
                if (other.name != null)
                    return false;
            }
            else if (!name.equals(other.name))
                return false;
            return true;
        }
    }

    protected static class ResultItemAttributeNode extends NameValueNode {
        private String methodName;

        public ResultItemAttributeNode(String name, String value) {
            super(name, value);
            methodName = "get" + StringUtils.capitalize(name.toLowerCase());
        }

        @Override
        public boolean matches(List<Node> path, long id, ResultItem resultItem) {
            try {
                Method method = resultItem.getClass().getMethod(methodName);
                return value.equals(String.valueOf(method.invoke(resultItem)));
            }
            catch (Exception e) {
                return false;
            }
        }
    }
}
