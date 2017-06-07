package org.omnetpp.scave.computed;

import java.text.ParseException;
import java.util.List;
import java.util.concurrent.Callable;

import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.omnetpp.scave.Markers;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.computed.ComputedScalarEngine.ComputedScalar;
import org.omnetpp.scave.computed.ExpressionEvaluator.EvaluationException;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.script.ComputeScalarCommand;

/**
 * Keeps computed scalars up-to-date.
 *
 * @author tomi
 */
public class ComputedScalarManager {
//                                     implements INotifyChangedListener, IResultFilesChangeListener {
//    private ResultFileManagerEx resultFileManager;
//    private IChangeNotifier changeNotifier;
//    private IFile analysisFile; // for markers
//
//    public ComputedScalarManager() {
//    }
//
//    public void init(IScaveEditorContext context, IFile analysisFile) {
//        this.resultFileManager = context.getResultFileManager();
//        this.changeNotifier = context.getChangeNotifier();
//        this.analysisFile = analysisFile;
//        Assert.isNotNull(analysisFile);
//
//        resultFileManager.addChangeListener(this);
//        changeNotifier.addListener(this);
//    }
//
//    public void dispose() {
//        resultFileManager.removeChangeListener(this);
//        changeNotifier.removeListener(this);
//    }
//
//    /**
//     * Utility method for accessing the manager associated with a computation node.
//     */
//    public static ComputedScalarManager instanceFor(ComputeScalar computation) {
//        IScaveEditorContext context = ScaveModelUtil.getScaveEditorContextFor(computation);
//        ComputedScalarManager manager = context.getComputedScalarManager();
//        Assert.isNotNull(manager);
//        return manager;
//    }
//
//    /**
//     * Returns the IDs of scalars computed by the specified operation.
//     * <p>
//     * If the result has already been computed, then the method queries the ResultFileManager,
//     * otherwise it performs the computation and stores the IDs
//     * in the ResultFileManager.
//     *
//     * @param op the computation node
//     * @param input the input statistics of the computation
//     * @return the IDs of the computed scalars
//     */
//    public IDList getOrCreateComputedScalars(ComputeScalar op, IDList input)
//    {
//        resultFileManager.checkReadLock();
//        IDList output = resultFileManager.getComputedScalarIDs(op);
//        if (output.isEmpty())
//        {
//            output = computeAndAddScalars(op, input);
//        }
//        return output;
//    }

    /**
     * Computes the scalars specified by the computation, and stores
     * the result in the ResultFileManager. If an error occurs, then
     * an error marker is added to the analysis file.
     *
     * @param op the computation node
     * @param input the input statistics of the computation
     * @return the IDs of the computed scalars
     */
    public static IDList computeAndAddScalars(ResultFileManager resultFileManager, final ComputeScalarCommand op, final IDList input) throws ParseException
    {
        //ComputeScalarProblemMarkerSynchronizer markers = new ComputeScalarProblemMarkerSynchronizer(Markers.COMPUTESCALAR_PROBLEMMARKER_ID, op);
//        try {
            final List<ComputedScalar> scalars = new ComputedScalarEngine(resultFileManager).computeScalars(op, input);

            IDList result = ResultFileManager.callWithWriteLock(resultFileManager, new Callable<IDList>() {
                public IDList call() throws Exception {
                    IDList idlist = new IDList();
                    for (ComputedScalar scalar : scalars) {
                        long id = resultFileManager.addComputedScalar(scalar.name, scalar.module, scalar.runName, scalar.value, scalar.runAttributes, op);
                        idlist.add(id);
                    }
                    return idlist;
                }
            });
            return result;
//        } catch (ParseException e) {
//            //TODO markers.addError(e.getMessage(), e.getErrorOffset());
//            return new IDList();
//        } catch (EvaluationException e) {
//            //TODO markers.addError(e.getMessage(), -1);
//            return new IDList();
//        } catch (Exception e) {
//            ScavePlugin.logError(e);
//            return new IDList();
//        } finally {
//            //TODO markers.runAsWorkspaceJob();
//        }
    }

//    /**
//     * Invalidate all computed scalars if a Dataset or Dataset item changed
//     * and the change influence the ComputedScalar node or the input of the computation.
//     */
//    public void notifyChanged(Notification notification) {
//        Object notifier = notification.getNotifier();
//        if (notification.isTouch())
//            return;
//        boolean canChangeComputedScalars =
//                    notifier instanceof Dataset || notifier instanceof Datasets ||
//                    (notifier instanceof DatasetItem && !(notifier instanceof Chart ||
//                                                          notifier instanceof Property ||
//                                                          notifier instanceof ProcessingOp ||
//                                                          notifier instanceof Param));
//
//        if (canChangeComputedScalars) {
//            resultFileManager.clearComputedScalars();
//            removeMarkers();
//        }
//    }
//
//    /**
//     * Invalidate all computed scalars if a result file changed.
//     */
//    public void resultFileManagerChanged(ResultFileManagerChangeEvent event) {
//        switch (event.getChangeType()) {
//        case LOAD:
//        case UNLOAD:
//            resultFileManager.clearComputedScalars();
//            removeMarkers();
//            break;
//        }
//    }
//
//    /**
//     * Remove error markers associated with computed scalars.
//     */
//    public void removeMarkers() {
//        ComputeScalarProblemMarkerSynchronizer markers = new ComputeScalarProblemMarkerSynchronizer(Markers.COMPUTESCALAR_PROBLEMMARKER_ID);
//        markers.runAsWorkspaceJob();
//    }
//
//    /**
//     * Utility class to manage error markers that belong to a ComputeScalar node.
//     * Markers will contain a reference to the computation node, so clicking
//     * on the error in the Problems view can navigate to the node.
//     * A readable location attribute is also added to the markers.
//     */
//    private class ComputeScalarProblemMarkerSynchronizer extends ProblemMarkerSynchronizer {
//
//        private ComputeScalar op;
//        private String location;
//
//        public ComputeScalarProblemMarkerSynchronizer(String markerBaseType) {
//            super(markerBaseType);
//            register(analysisFile);
//        }
//
//        public ComputeScalarProblemMarkerSynchronizer(String markerBaseType, ComputeScalar op) {
//            super(markerBaseType);
//            Assert.isNotNull(op);
//            this.op = op;
//            register(analysisFile);
//        }
//
//        void addError(String message, int charStart) {
//            Map<String, Object> map = new HashMap<String, Object>();
//            map.put(IMarker.SEVERITY, IMarker.SEVERITY_ERROR);
//            map.put(IMarker.MESSAGE, message);
//            if (charStart != -1)
//                map.put(IMarker.CHAR_START, charStart);
//            if (op != null) {
//                map.put(Markers.EOBJECT_MARKERATTR_ID, op);
//                map.put(IMarker.LOCATION, getLocation());
//            }
//            addMarker(analysisFile, getBaseMarkerType(), map);
//        }
//
//        /**
//         * Computes the location attribute of the ComputeScalar node.
//         * The location is in the form of "dataset <name>/compute scalar: <name>".
//         */
//        private String getLocation() {
//            if (location == null) {
//                Dataset dataset = ScaveModelUtil.findEnclosingDataset(op);
//                String scalarName = op.getScalarName();
//
//                StringBuilder sb = new StringBuilder();
//                sb.append("dataset ");
//                if (dataset == null)
//                    sb.append("<unknown>");
//                else if (StringUtils.isEmpty(dataset.getName()))
//                    sb.append("<unnamed>");
//                else
//                    sb.append(dataset.getName());
//                sb.append("/compute scalar: ").append(StringUtils.isEmpty(scalarName) ? "<unnamed>" : scalarName);
//                location = sb.toString();
//            }
//            return location;
//        }
//
//        @Override
//        protected boolean isManagedByThisSynchronizer(IMarker marker) throws CoreException {
//            return op == null || op == marker.getAttribute(Markers.EOBJECT_MARKERATTR_ID);
//        }
//    }
}
