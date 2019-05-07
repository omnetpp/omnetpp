package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.charting.dataset.VectorDataLoader;
import org.omnetpp.scave.charting.dataset.XYVector;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.VectorResult;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class VectorResultsPickler implements IObjectPickler {

    void pickleVectorResult(ResultFileManager resultManager, long ID, XYVector data, Pickler pickler,
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

            ResultPicklingUtils.pickleDoubleArray(data.x, out);
            ResultPicklingUtils.pickleDoubleArray(data.y, out);
        }
        out.write(Opcodes.TUPLE);
    }

    String filterExpression;
    boolean includeAttrs;
    boolean includeRunattrs;
    boolean includeItervars;
    boolean mergeModuleAndName;

    public VectorResultsPickler(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) {
        this.filterExpression = filterExpression;
        this.includeAttrs = includeAttrs;
        this.includeRunattrs = includeRunattrs;
        this.includeItervars = includeItervars;
        this.mergeModuleAndName = mergeModuleAndName;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;

        out.write(Opcodes.MARK);
        {
            out.write(Opcodes.MARK);
            if (filterExpression != null && !filterExpression.trim().isEmpty()) {
                IDList vectors = resultManager.getAllVectors();
                vectors = resultManager.filterIDList(vectors, filterExpression);

                Debug.println("pickling " + vectors.size() + " vectors");

                XYVector[] vectorsData = VectorDataLoader.getDataOfVectors(resultManager, vectors, null);

                for (int i = 0; i < vectors.size(); ++i)
                    pickleVectorResult(resultManager, vectors.get(i), vectorsData[i], pickler, out);
            }
            out.write(Opcodes.LIST);

            if (includeAttrs)
                new ResultAttrsPickler(filterExpression).pickle(resultManager, out, pickler);
            else
                out.write(Opcodes.NONE);

            if (includeRunattrs)
                new RunAttrsPickler(filterExpression, RunAttrsPickler.FilterMode.FILTER_RESULTS).pickle(resultManager, out, pickler);
            else
                out.write(Opcodes.NONE);

            if (includeItervars)
                new IterVarsPickler(filterExpression, IterVarsPickler.FilterMode.FILTER_RESULTS).pickle(resultManager, out, pickler);
            else
                out.write(Opcodes.NONE);
        }
        out.write(Opcodes.TUPLE);
    }
}
