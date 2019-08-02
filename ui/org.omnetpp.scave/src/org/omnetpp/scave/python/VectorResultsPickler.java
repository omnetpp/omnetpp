package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.charting.dataset.VectorDataLoader;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScaveEngine;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.engine.XYArrayVector;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class VectorResultsPickler implements IObjectPickler {

    void pickleVectorResult(ResultFileManager resultManager, long ID, XYArray data, Pickler pickler,
            OutputStream out) throws PickleException, IOException {
        VectorResult result = resultManager.getVector(ID);

        // runID, module, name, vectime, vecvalue
        out.write(Opcodes.MARK);
        {
            pickler.save(result.getRun().getRunName());
            pickler.save(result.getModuleName());
            if (mergeModuleAndName)
                pickler.save(result.getModuleName() + "." + result.getName());
            else
                pickler.save(result.getName());

            ResultPicklingUtils.pickleXYArray(data, out);
        }
        out.write(Opcodes.TUPLE);
    }

    String filterExpression;
    boolean includeAttrs;
    boolean mergeModuleAndName;
    double simTimeStart;
    double simTimeEnd;
    InterruptedFlag interruptedFlag;

    public VectorResultsPickler(String filterExpression, boolean includeAttrs, boolean mergeModuleAndName, double simTimeStart, double simTimeEnd, InterruptedFlag interruptedFlag) {
        this.filterExpression = filterExpression;
        this.includeAttrs = includeAttrs;
        this.mergeModuleAndName = mergeModuleAndName;
        this.simTimeStart = simTimeStart;
        this.simTimeEnd = simTimeEnd;
        this.interruptedFlag = interruptedFlag;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;

        out.write(Opcodes.MARK);
        {
            IDList vectors = null;

            out.write(Opcodes.MARK);
            if (filterExpression != null && !filterExpression.trim().isEmpty()) {
                if (ResultPicklingUtils.debug)
                    Debug.println("vector pickling start");

                vectors = resultManager.getAllVectors();
                vectors = resultManager.filterIDList(vectors, filterExpression, interruptedFlag);

                if (ResultPicklingUtils.debug)
                    Debug.println("pickling " + vectors.size() + " vectors");

                XYArrayVector vectorsData = VectorDataLoader.getDataOfVectors(resultManager, vectors, simTimeStart, simTimeEnd, interruptedFlag);

                for (int i = 0; i < vectors.size(); ++i) {
                    pickleVectorResult(resultManager, vectors.get(i), vectorsData.get(i), pickler, out);
                    if (interruptedFlag.getFlag())
                        throw new RuntimeException("Result pickling interrupted");
                }
                if (ResultPicklingUtils.debug)
                    Debug.println("vector pickling done, cleaning up memory");

                vectorsData.delete();
                vectorsData = null;
                //System.gc(); // NOT NEEDED, SLOW, and actually BREAKS some internal parts of Py4J...
                ScaveEngine.malloc_trim(); // so the std::vector buffers (in vectorsData) are released to the operating system

                if (ResultPicklingUtils.debug)
                    Debug.println("vector data cleanup done");
            }
            out.write(Opcodes.LIST);

            if (vectors != null && includeAttrs)
                new ResultAttrsPickler(vectors, interruptedFlag).pickle(resultManager, out, pickler);
            else
                out.write(Opcodes.NONE);
        }
        out.write(Opcodes.TUPLE);
    }
}
