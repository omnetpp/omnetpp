package org.omnetpp.scave.editors.datatable;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;

import org.apache.commons.collections.map.MultiValueMap;
import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.lang3.text.WordUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.Debug;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ui.ScaveUtil;
import org.omnetpp.scave.engine.DoubleVector;
import org.omnetpp.scave.engine.FileRun;
import org.omnetpp.scave.engine.Histogram;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunAttribute;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.Statistics;
import org.omnetpp.scave.engine.StatisticsResult;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

/**
 * This is the content provider used with the DataTree widget.
 *
 * This class provides a customizable tree of various data from the result file manager.
 * The tree is built up from levels that may be freely reordered and switched on/off.
 * Levels include experiment, measurement, replication, config, run number, file name, run id, module path, module name, result item, result item attributes, etc.
 *
 * @author levy
 */
@SuppressWarnings({"unchecked", "rawtypes"})
public class DataTreeContentProvider {
    public final static Class[] LEVELS1 = new Class[] { ExperimentNode.class, MeasurementNode.class, ReplicationNode.class, ModulePathNode.class, ResultItemNode.class, ResultItemAttributeNode.class};
    public final static Class[] LEVELS2 = new Class[] { ExperimentMeasurementReplicationNode.class, ModulePathNode.class, ResultItemNode.class, ResultItemAttributeNode.class};
    public final static Class[] LEVELS3 = new Class[] { ConfigNode.class, RunNumberNode.class, ModulePathNode.class, ResultItemNode.class, ResultItemAttributeNode.class};
    public final static Class[] LEVELS4 = new Class[] { ConfigRunNumberNode.class, ModulePathNode.class, ResultItemNode.class, ResultItemAttributeNode.class};
    public final static Class[] LEVELS5 = new Class[] { FileNameNode.class, RunIdNode.class, ModulePathNode.class, ResultItemNode.class, ResultItemAttributeNode.class};
    public final static Class[] LEVELS6 = new Class[] { RunIdNode.class, ModulePathNode.class, ResultItemNode.class, ResultItemAttributeNode.class};

    private static boolean debug = true;

    protected ResultFileManagerEx manager;

    protected IDList idList;

    protected Class<? extends Node>[] levels;

    protected int numericPrecision = 6;

    protected Node[] rootNodes;

    public DataTreeContentProvider() {
        setDefaultLevels();
    }

    public void setResultFileManager(ResultFileManagerEx manager) {
        this.manager = manager;
        rootNodes = null;
    }

    public void setIDList(IDList idList) {
        this.idList = idList;
        rootNodes = null;
    }

    public Class<? extends Node>[] getLevels() {
        return levels;
    }

    public void setLevels(Class<? extends Node>[] levels) {
        if (debug)
            Debug.println("setLevels(): " + levels);
        this.levels = levels;
        rootNodes = null;
    }

    public void setDefaultLevels() {
        setLevels(LEVELS2);
    }

    public void setNumericPrecision(int numericPrecision) {
        this.numericPrecision = numericPrecision; // note: corresponding refresh() call is in DataTree
    }

    public int getNumericPrecision() {
        return numericPrecision;
    }

    public Node[] getChildNodes(final List<Node> path) {
        long startMillis = System.currentTimeMillis();
        if (manager == null || idList == null)
            return new Node[0];
        final Node firstNode = path.size() == 0 ? null : path.get(0);
        // cache
        if (firstNode == null) {
            if (rootNodes != null)
                return rootNodes;
        }
        else {
            if (firstNode.children != null)
                return firstNode.children;
        }
        Node[] nodes = ResultFileManager.callWithReadLock(manager, new Callable<Node[]>() {
            public Node[] call() throws Exception {
                MultiValueMap nodeIdsMap = MultiValueMap.decorate(new LinkedHashMap<>()); // preserve insertion order of children
                int currentLevelIndex;
                if (firstNode == null)
                    currentLevelIndex = -1;
                else {
                    currentLevelIndex = ArrayUtils.indexOf(levels, firstNode.getClass());
                    if (currentLevelIndex == -1)
                        return new Node[0];
                }
                int nextLevelIndex;
                if (firstNode instanceof ModuleNameNode) {
                    ModuleNameNode moduleNameNode = (ModuleNameNode)firstNode;
                    nextLevelIndex = currentLevelIndex + (moduleNameNode.leaf ? 1 : 0);
                }
                else
                    nextLevelIndex = currentLevelIndex + 1;
                boolean collector = false;
                for (int j = nextLevelIndex + 1; j < levels.length; j++)
                    if (!levels[j].equals(ResultItemAttributeNode.class))
                        collector = true;
                Class<? extends Node> nextLevelClass = nextLevelIndex < levels.length ? levels[nextLevelIndex] : null;
                boolean shouldSort = true;
                if (nextLevelClass != null) {
                    int idCount = firstNode == null ? idList.size() : firstNode.ids.length;
                    for (int i = 0; i < idCount; i++) {
                        long id = firstNode == null ? idList.get(i) : firstNode.ids[i];
                        MatchContext matchContext = new MatchContext(manager, id);
                        if (matchesPath(path, id, matchContext)) {
                            if (nextLevelClass.equals(ExperimentNode.class))
                                nodeIdsMap.put(new ExperimentNode(matchContext.getRunAttribute(RunAttribute.EXPERIMENT)), id);
                            else if (nextLevelClass.equals(MeasurementNode.class))
                                nodeIdsMap.put(new MeasurementNode(matchContext.getRunAttribute(RunAttribute.MEASUREMENT)), id);
                            else if (nextLevelClass.equals(ReplicationNode.class))
                                nodeIdsMap.put(new ReplicationNode(matchContext.getRunAttribute(RunAttribute.REPLICATION)), id);
                            else if (nextLevelClass.equals(ExperimentMeasurementReplicationNode.class))
                                nodeIdsMap.put(new ExperimentMeasurementReplicationNode(matchContext.getRunAttribute(RunAttribute.EXPERIMENT), matchContext.getRunAttribute(RunAttribute.MEASUREMENT), matchContext.getRunAttribute(RunAttribute.REPLICATION)), id);
                            else if (nextLevelClass.equals(ConfigNode.class))
                                nodeIdsMap.put(new ConfigNode(matchContext.getRunAttribute(RunAttribute.CONFIGNAME)), id);
                            else if (nextLevelClass.equals(RunNumberNode.class))
                                nodeIdsMap.put(new RunNumberNode(matchContext.getRun().getAttribute(RunAttribute.RUNNUMBER)), id);
                            else if (nextLevelClass.equals(ConfigRunNumberNode.class))
                                nodeIdsMap.put(new ConfigRunNumberNode(matchContext.getRunAttribute(RunAttribute.CONFIGNAME), matchContext.getRun().getAttribute(RunAttribute.RUNNUMBER)), id);
                            else if (nextLevelClass.equals(FileNameNode.class))
                                nodeIdsMap.put(new FileNameNode(matchContext.getResultFile().getFileName()), id);
                            else if (nextLevelClass.equals(RunIdNode.class))
                                nodeIdsMap.put(new RunIdNode(matchContext.getRun().getRunName()), id);
                            else if (nextLevelClass.equals(FileNameRunIdNode.class))
                                nodeIdsMap.put(new FileNameRunIdNode(matchContext.getResultFile().getFileName(), matchContext.getRun().getRunName()), id);
                            else if (nextLevelClass.equals(ModuleNameNode.class)) {
                                String moduleName = matchContext.getResultItem().getModuleName();
                                String modulePrefix = getModulePrefix(path, null);
                                if (moduleName.startsWith(modulePrefix)) {
                                    String remainingName = StringUtils.removeStart(StringUtils.removeStart(moduleName, modulePrefix), ".");
                                    String name = StringUtils.substringBefore(remainingName, ".");
                                    nodeIdsMap.put(new ModuleNameNode(StringUtils.isEmpty(name) ? "." : name, !remainingName.contains(".")), id);
                                }
                            }
                            else if (nextLevelClass.equals(ModulePathNode.class))
                                nodeIdsMap.put(new ModulePathNode(matchContext.getResultItem().getModuleName()), id);
                            else if (nextLevelClass.equals(ResultItemNode.class)) {
                                if (collector)
                                    nodeIdsMap.put(new ResultItemNode(manager, -1, matchContext.getResultItem().getName()), id);
                                else
                                    nodeIdsMap.put(new ResultItemNode(manager, id, null), id);
                            }
                            else if (nextLevelClass.equals(ResultItemAttributeNode.class)) {
                                shouldSort = false;  // retain insertion order
                                ResultItem resultItem = matchContext.getResultItem();
                                ResultItem.DataType type = resultItem.getDataType();
                                boolean isIntegerType = type == ResultItem.DataType.TYPE_INT;
                                nodeIdsMap.put(new ResultItemAttributeNode("Module name", resultItem.getModuleName()), id);
                                nodeIdsMap.put(new ResultItemAttributeNode("Type", type.toString().replaceAll("TYPE_", "").toLowerCase()), id);
                                if (resultItem instanceof ScalarResult) {
                                    ScalarResult scalar = (ScalarResult)resultItem;
                                    nodeIdsMap.put(new ResultItemAttributeNode("Value", toIntegerAwareString(scalar.getValue(), isIntegerType)), id);
                                }
                                else if (resultItem instanceof VectorResult) {
                                    VectorResult vector = (VectorResult)resultItem;
                                    Statistics stat = vector.getStatistics();
                                    nodeIdsMap.put(new ResultItemAttributeNode("Count", String.valueOf(stat.getCount())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Mean", formatNumber(stat.getMean())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("StdDev", formatNumber(stat.getStddev())), id);
                                    //nodeIdsMap.put(new ResultItemAttributeNode("Variance", String.valueOf(stat.getVariance())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Min", toIntegerAwareString(stat.getMin(), isIntegerType)), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Max", toIntegerAwareString(stat.getMax(), isIntegerType)), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Start event number", String.valueOf(vector.getStartEventNum())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("End event number", String.valueOf(vector.getEndEventNum())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Start time", formatNumber(vector.getStartTime())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("End time", formatNumber(vector.getEndTime())), id);
                                }
                                else if (resultItem instanceof StatisticsResult) {
                                    StatisticsResult statistics = (StatisticsResult)resultItem;
                                    Statistics stat = statistics.getStatistics();
                                    nodeIdsMap.put(new ResultItemAttributeNode("Kind", (stat.isWeighted() ? "weighted" : "unweighted")), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Count", String.valueOf(stat.getCount())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Sum of weights", formatNumber(stat.getSumWeights())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Mean", formatNumber(stat.getMean())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("StdDev", formatNumber(stat.getStddev())), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Min", toIntegerAwareString(stat.getMin(), isIntegerType)), id);
                                    nodeIdsMap.put(new ResultItemAttributeNode("Max", toIntegerAwareString(stat.getMax(), isIntegerType)), id);

                                    if (resultItem instanceof HistogramResult) {
                                        HistogramResult histogram = (HistogramResult)resultItem;
                                        Histogram hist = histogram.getHistogram();
                                        DoubleVector binEdges = hist.getBinLowerBounds();
                                        DoubleVector binValues = hist.getBinValues();
                                        if (binEdges.size() > 0 && binValues.size() > 0) {
                                            ResultItemAttributeNode binsNode = new ResultItemAttributeNode("Bins", String.valueOf(hist.getNumBins()-2)); //TODO "-2"
                                            List<Node> list = new ArrayList<Node>();
                                            for (int j = 0; j < binEdges.size(); j++) {
                                                double lowerBound = binEdges.get(j);
                                                double upperBound = j < binEdges.size()-1 ? binEdges.get(j+1) : Double.POSITIVE_INFINITY;
                                                double value = binValues.get(j);
                                                String name = "[" + toIntegerAwareString(lowerBound, isIntegerType) + ", ";
                                                if (isIntegerType)
                                                    name += toIntegerAwareString(upperBound-1, isIntegerType) + "]";
                                                else
                                                    name += String.valueOf(upperBound) + ")";
                                                list.add(new NameValueNode(name, toIntegerAwareString(value, true)));
                                            }
                                            binsNode.children = list.toArray(new Node[0]);
                                            nodeIdsMap.put(binsNode, id);
                                        }
                                    }
                                }
                                else {
                                    throw new IllegalArgumentException();
                                }
                                StringMap attributes = resultItem.getAttributes();
                                StringVector keys = attributes.keys();
                                for (int j = 0; j < keys.size(); j++) {
                                    String key = keys.get(j);
                                    nodeIdsMap.put(new ResultItemAttributeNode(StringUtils.capitalize(key), attributes.get(key)), id);
                                }
                            }
                            else
                                throw new IllegalArgumentException();
                        }
                    }
                }
                Node[] nodes = (Node[])nodeIdsMap.keySet().toArray(new Node[0]);
                if (shouldSort) {
                    Arrays.sort(nodes, new Comparator<Node>() {
                        public int compare(Node o1, Node o2) {
                            return StringUtils.dictionaryCompare((o1).getColumnText(0), (o2).getColumnText(0));
                        }
                    });
                }
                for (Node node : nodes) {
                    node.ids = toLongArray((Collection<Long>)nodeIdsMap.getCollection(node));
                    // add quick value if applicable
                    if (node.ids.length == 1 && !collector && StringUtils.isEmpty(node.value) &&
                        (!(node instanceof ModuleNameNode) || ((ModuleNameNode)node).leaf))
                    {
                        ResultItem resultItem = manager.getItem(node.ids[0]);
                        node.value = getResultItemShortDescription(resultItem);
                    }
                }
                // update cache
                if (firstNode == null)
                    rootNodes = nodes;
                else
                    firstNode.children = nodes;
                return nodes;
            }

            private long[] toLongArray(Collection<Long> c) {
                long[] a = new long[c.size()];
                Iterator<Long> it = c.iterator();
                for (int i = 0; i < c.size(); i++)
                    a[i] = (Long)it.next();
                return a;
            }

            protected String toIntegerAwareString(double value, boolean isIntegerType) {
                if (!isIntegerType || Double.isInfinite(value) || Double.isNaN(value) || Math.floor(value) != value)
                    return formatNumber(value);
                else
                    return String.valueOf((long)value);
            }


        });

        long totalMillis = System.currentTimeMillis() - startMillis;
        if (debug)
            Debug.println("getChildNodes() for path = " + path + ": " + totalMillis + "ms");

        return nodes;
    }

    protected String formatNumber(double d) {
        return ScaveUtil.formatNumber(d, getNumericPrecision());
    }

    protected String formatNumber(BigDecimal d) {
        return ScaveUtil.formatNumber(d, getNumericPrecision());
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

    protected String getResultItemShortDescription(ResultItem resultItem) {
        if (resultItem instanceof ScalarResult) {
            ScalarResult scalar = (ScalarResult)resultItem;
            return formatNumber(scalar.getValue());
        }
        else if (resultItem instanceof VectorResult) {
            VectorResult vector = (VectorResult)resultItem;
            Statistics stat = vector.getStatistics();
            return formatNumber(stat.getMean()) + " (" + String.valueOf(stat.getCount()) + ")";
        }
        else if (resultItem instanceof HistogramResult) {  // note: should precede StatisticsResult branch
            HistogramResult histogram = (HistogramResult)resultItem;
            Statistics stat = histogram.getStatistics();
            Histogram hist = histogram.getHistogram();
            return formatNumber(stat.getMean()) + " (" + String.valueOf(stat.getCount()) + ") [" + (hist.getNumBins()-2) + " bins]"; //TODO "-2"
        }
        else if (resultItem instanceof StatisticsResult) {
            StatisticsResult histogram = (StatisticsResult)resultItem;
            Statistics stat = histogram.getStatistics();
            return formatNumber(stat.getMean()) + " (" + String.valueOf(stat.getCount()) + ")";
        }
        else
            throw new IllegalArgumentException();
    }

    protected static String getResultItemReadableClassName(ResultItem resultItem) {
        return resultItem.getClass().getSimpleName().replaceAll("Result", "").toLowerCase();
    }

    protected static class MatchContext {
        private ResultFileManager manager;
        private long id;
        private ResultItem resultItem;
        private FileRun fileRun;
        private ResultFile resultFile;
        private Run run;

        public MatchContext(ResultFileManager manager, long id) {
            this.manager = manager;
            this.id = id;
        }

        public String getRunAttribute(String key) {
            return StringUtils.defaultString(manager.getRunAttribute(id, key), "?");
        }

        public ResultItem getResultItem() {
            if (resultItem == null)
                resultItem = manager.getItem(id);
            return resultItem;
        }
        public FileRun getFileRun() {
            if (fileRun == null)
                fileRun = getResultItem().getFileRun();
            return fileRun;
        }

        public ResultFile getResultFile() {
            if (resultFile == null)
                resultFile = getFileRun().getFile();
            return resultFile;
        }
        public Run getRun() {
            if (run == null)
                run = getFileRun().getRun();
            return run;
        }
    }

    protected boolean matchesPath(List<Node> path, long id, MatchContext matchContext) {
        for (Node node : path)
            if (!node.matches(path, id, matchContext))
                return false;
        return true;
    }

    /* Various tree node types */

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
            ResultItemAttributeNode.class};
    }

    private static final Map<Class,String> LEVELNAMES = new HashMap<>();
    static {
        LEVELNAMES.put(ExperimentNode.class, "Experiment");
        LEVELNAMES.put(MeasurementNode.class, "Measurement");
        LEVELNAMES.put(ReplicationNode.class, "Replication");
        LEVELNAMES.put(ExperimentMeasurementReplicationNode.class, "Experiment + Measurement + Replication");
        LEVELNAMES.put(ConfigNode.class, "Config");
        LEVELNAMES.put(RunNumberNode.class, "Run Number");
        LEVELNAMES.put(ConfigRunNumberNode.class, "Config + Run Number");
        LEVELNAMES.put(FileNameNode.class, "File");
        LEVELNAMES.put(RunIdNode.class, "Run Id");
        LEVELNAMES.put(FileNameRunIdNode.class, "File + Run Id");
        LEVELNAMES.put(ModulePathNode.class, "Module Path");
        LEVELNAMES.put(ModuleNameNode.class, "Module Name");
        LEVELNAMES.put(ResultItemNode.class, "Result Item");
        LEVELNAMES.put(ResultItemAttributeNode.class, "Result Item Attribute");
    }

    public static String getLevelName(Class levelClass) {
        return LEVELNAMES.get(levelClass);
    }

    protected static abstract class Node {
        public long[] ids;

        public Node[] children;

        public String value = "";

        public boolean isExpandedByDefault() {
            return false;
        }

        public abstract Image getImage();

        public abstract String getColumnText(int index);

        public abstract boolean matches(List<Node> path, long id, MatchContext matchContext);
    }

    public static class NameValueNode extends Node {
        public String name;

        public NameValueNode(String name, String value) {
            this.name = name;
            this.value = value;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? name : value;
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            return true;
        }

        @Override
        public Image getImage() {
            return ImageFactory.global().getIconImage(ImageFactory.TOOLBAR_IMAGE_PROPERTIES);
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

    public static class ExperimentNode extends Node {
        public String name;

        public ExperimentNode(String name) {
            this.name = name;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? name + " (experiment)" : value;
        }

        public Image getImage() {
            return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_EXPERIMENT);
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            return name.equals(matchContext.getRunAttribute(RunAttribute.EXPERIMENT));
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

    public static class MeasurementNode extends Node {
        public String name;

        public MeasurementNode(String name) {
            this.name = name;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? StringUtils.defaultIfEmpty(name, "default")  + " (measurement)" : value;
        }

        public Image getImage() {
            return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_MEASUREMENT);
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            return name.equals(matchContext.getRunAttribute(RunAttribute.MEASUREMENT));
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

    public static class ReplicationNode extends Node {
        public String name;

        public ReplicationNode(String name) {
            this.name = name;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? name + " (replication)" : value;
        }

        public Image getImage() {
            return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_REPLICATION);
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            return name.equals(matchContext.getRunAttribute(RunAttribute.REPLICATION));
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

    public static class ExperimentMeasurementReplicationNode extends Node {
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
            return index == 0 ? experiment + (StringUtils.isEmpty(measurement) ? "" : " : " + measurement) + " : " + replication : value;
        }

        public Image getImage() {
            return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_RUN);
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            return experiment.equals(matchContext.getRunAttribute(RunAttribute.EXPERIMENT)) && measurement.equals(matchContext.getRunAttribute(RunAttribute.MEASUREMENT)) && replication.equals(matchContext.getRunAttribute(RunAttribute.REPLICATION));
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

    public static class ConfigNode extends Node {
        public String name;

        public ConfigNode(String name) {
            this.name = name;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? name + " (config)" : value;
        }

        public Image getImage() {
            return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_CONFIGURATION);
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            return matchContext.getRunAttribute(RunAttribute.CONFIGNAME).equals(name);
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

    public static class RunNumberNode extends Node {
        public String runNumber;

        public RunNumberNode(String runNumber) {
            this.runNumber = runNumber;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? runNumber + " (run number)" : value;
        }

        public Image getImage() {
            return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_RUNNUMBER);
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            return matchContext.getRunAttribute(RunAttribute.RUNNUMBER).equals(runNumber);
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((runNumber == null) ? 0 : runNumber.hashCode());
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
            if (runNumber == null) {
                if (other.runNumber != null)
                    return false;
            } else if (!runNumber.equals(other.runNumber))
                return false;
            return true;
        }

    }

    public static class ConfigRunNumberNode extends Node {
        public String config;
        public String runNumber;

        public ConfigRunNumberNode(String config, String runNumber) {
            this.config = config;
            this.runNumber = runNumber;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? config + " - #" + runNumber : value;
        }

        public Image getImage() {
            return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_RUN);
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            return matchContext.getRunAttribute(RunAttribute.CONFIGNAME).equals(config) && matchContext.getRunAttribute(RunAttribute.RUNNUMBER).equals(runNumber);
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((config == null) ? 0 : config.hashCode());
            result = prime * result + ((runNumber == null) ? 0 : runNumber.hashCode());
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
            } else if (!config.equals(other.config))
                return false;
            if (runNumber == null) {
                if (other.runNumber != null)
                    return false;
            } else if (!runNumber.equals(other.runNumber))
                return false;
            return true;
        }

    }

    public static class FileNameNode extends Node {
        public String fileName;

        public FileNameNode(String name) {
            this.fileName = name;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? fileName + " (file name)" : value;
        }

        public Image getImage() {
            return ScavePlugin.getCachedImage(fileName.endsWith(".vec") ? ScaveImages.IMG_VECFILE : ScaveImages.IMG_SCAFILE);
        }

        public boolean isExpandedByDefault() {
            return true;
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            return matchContext.getResultFile().getFileName().equals(fileName);
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((fileName == null) ? 0 : fileName.hashCode());
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
            if (fileName == null) {
                if (other.fileName != null)
                    return false;
            }
            else if (!fileName.equals(other.fileName))
                return false;
            return true;
        }
    }

    public static class RunIdNode extends Node {
        public String runId;

        public RunIdNode(String runId) {
            this.runId = runId;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? runId + " (run id)" : value;
        }

        public Image getImage() {
            return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_RUN);
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            return matchContext.getRun().getRunName().equals(runId);
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
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
            RunIdNode other = (RunIdNode) obj;
            if (runId == null) {
                if (other.runId != null)
                    return false;
            }
            else if (!runId.equals(other.runId))
                return false;
            return true;
        }
    }

    public static class FileNameRunIdNode extends Node {
        public String fileName;

        public String runId;

        public FileNameRunIdNode(String fileName, String runId) {
            this.fileName = fileName;
            this.runId = runId;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? fileName + " : " + runId : value;
        }

        public Image getImage() {
            return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_RUN);
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            return matchContext.getResultFile().getFileName().equals(fileName) && matchContext.getRun().getRunName().equals(runId);
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

    public static class ModulePathNode extends Node {
        public String path;

        public ModulePathNode(String path) {
            this.path = path;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? path : value;
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            return matchContext.getResultItem().getModuleName().equals(this.path);
        }

        @Override
        public Image getImage() {
            return ImageFactory.global().getIconImage(ImageFactory.MODEL_IMAGE_SIMPLEMODULE);
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

    public static class ModuleNameNode extends Node {
        public String name;

        public boolean leaf;

        public ModuleNameNode(String name, boolean leaf) {
            this.name = name;
            this.leaf = leaf;
        }

        @Override
        public String getColumnText(int index) {
            return index == 0 ? name : value;
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            String modulePrefix = getModulePrefix(path, this);
            modulePrefix = StringUtils.isEmpty(modulePrefix) ? name : modulePrefix + "." + name;
            return matchContext.getResultItem().getModuleName().startsWith(modulePrefix);
        }

        @Override
        public Image getImage() {
            return ImageFactory.global().getIconImage(ImageFactory.MODEL_IMAGE_SIMPLEMODULE);
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

    public class ResultItemNode extends Node {  // note: non-static because it needs to call getResultItemShortDescription()
        public ResultFileManager manager;

        public long id;

        public String name;

        public ResultItemNode(ResultFileManager manager, long id, String name) {
            Assert.isTrue(id != -1 || name != null);
            this.manager = manager;
            this.id = id;
            this.name = name;
        }

        @Override
        public String getColumnText(int index) {
            if (name != null) {
                return index == 0 ? name : value;
            }
            else {
                ResultItem resultItem = manager.getItem(id);
                return index == 0 ? resultItem.getName() + " (" + getResultItemReadableClassName(resultItem) + ")" : getResultItemShortDescription(resultItem);
            }
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            if (name != null)
                return matchContext.getResultItem().getName().equals(name);
            else
                return this.id == id;
        }

        @Override
        public Image getImage() {
            if (name != null) {
                int allType = -1;
                for (long id : ids) {
                    int type = ResultFileManager.getTypeOf(id);
                    if (allType == -1)
                        allType = type;
                    else if (allType != type)
                        return ImageFactory.global().getIconImage(ImageFactory.MODEL_IMAGE_FOLDER);
                }
                if (allType == ResultFileManager.SCALAR)
                    return ImageFactory.global().getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWSCALARS);
                else if (allType == ResultFileManager.VECTOR)
                    return ImageFactory.global().getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWVECTORS);
                else if (allType == ResultFileManager.STATISTICS)
                    return ImageFactory.global().getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWHISTOGRAMS);
                else if (allType == ResultFileManager.HISTOGRAM)
                    return ImageFactory.global().getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWHISTOGRAMS);
                else
                    return ImageFactory.global().getIconImage(ImageFactory.DEFAULT);
            }
            else {
                ResultItem resultItem = manager.getItem(id);
                if (resultItem instanceof ScalarResult)
                    return ImageFactory.global().getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWSCALARS);
                else if (resultItem instanceof VectorResult)
                    return ImageFactory.global().getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWVECTORS);
                else if (resultItem instanceof StatisticsResult)
                    return ImageFactory.global().getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWHISTOGRAMS);
                else if (resultItem instanceof HistogramResult)
                    return ImageFactory.global().getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWHISTOGRAMS);
                else
                    return ImageFactory.global().getIconImage(ImageFactory.DEFAULT);
            }
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + (int) (id ^ (id >>> 32));
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
            ResultItemNode other = (ResultItemNode) obj;
            if (id != other.id)
                return false;
            if (name == null) {
                if (other.name != null)
                    return false;
            }
            else if (!name.equals(other.name))
                return false;
            return true;
        }
    }

    public static class ResultItemAttributeNode extends NameValueNode {
        private String methodName;

        public ResultItemAttributeNode(String name, String value) {
            super(name, value);
            methodName = "get" + WordUtils.capitalize(name.toLowerCase()).replaceAll(" ", "");
        }

        public static String getLevelName() {
            return "Result Item Attribute";
        }

        @Override
        public boolean matches(List<Node> path, long id, MatchContext matchContext) {
            try {
                ResultItem resultItem = matchContext.getResultItem();
                Method method = resultItem.getClass().getMethod(methodName);
                return value.equals(String.valueOf(method.invoke(resultItem)));
            }
            catch (Exception e) {
                return false;
            }
        }
    }
}
