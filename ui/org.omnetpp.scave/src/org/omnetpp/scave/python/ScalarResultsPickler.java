package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarResult;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class ScalarResultsPickler implements IObjectPickler {

    String filterExpression;
    boolean includeAttrs;
    boolean includeRunattrs;
    boolean includeItervars;
    boolean mergeModuleAndName;

    public ScalarResultsPickler(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) {
        this.filterExpression = filterExpression;
        this.includeAttrs = includeAttrs;
        this.includeRunattrs = includeRunattrs;
        this.includeItervars = includeItervars;
        this.mergeModuleAndName = mergeModuleAndName;
    }

    void pickleScalarResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        ScalarResult result = resultManager.getScalar(ID);

        // runID, module, name, value
        out.write(Opcodes.MARK);
        {
            pickler.save(result.getRun().getRunName());
            pickler.save(result.getModuleName());
            if (mergeModuleAndName)
                pickler.save(result.getModuleName() + "." + result.getName());
            else
                pickler.save(result.getName());
            pickler.save(result.getValue());
        }
        out.write(Opcodes.TUPLE);
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;

        out.write(Opcodes.MARK);
        {
            out.write(Opcodes.MARK);
            if (filterExpression != null && !filterExpression.trim().isEmpty()) {
                IDList scalars = resultManager.getAllScalars(false, false);
                scalars = resultManager.filterIDList(scalars, filterExpression);

                Debug.println("pickling " + scalars.size() + " scalars");
                for (int i = 0; i < scalars.size(); ++i)
                    pickleScalarResult(resultManager, scalars.get(i), pickler, out);
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
