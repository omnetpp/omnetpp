package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ParameterResult;
import org.omnetpp.scave.engine.ResultFileManager;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class ParamValuesPickler implements IObjectPickler {

    String filterExpression;
    boolean includeAttrs;
    boolean mergeModuleAndName;
    InterruptedFlag interruptedFlag;

    public ParamValuesPickler(String filterExpression, boolean includeAttrs, boolean mergeModuleAndName, InterruptedFlag interruptedFlag) {
        this.filterExpression = filterExpression;
        this.includeAttrs = includeAttrs;
        this.mergeModuleAndName = mergeModuleAndName;
        this.interruptedFlag = interruptedFlag;
    }

    void pickleParameterResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        ParameterResult result = resultManager.getParameter(ID);

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
            IDList params = null;

            out.write(Opcodes.MARK);
            if (filterExpression != null && !filterExpression.trim().isEmpty()) {
                params = resultManager.getAllParameters();
                params = resultManager.filterIDList(params, filterExpression, interruptedFlag);

                if (ResultPicklingUtils.debug)
                    Debug.println("pickling " + params.size() + " param values");
                for (int i = 0; i < params.size(); ++i) {
                    pickleParameterResult(resultManager, params.get(i), pickler, out);
                    if (i % 10 == 0 && interruptedFlag.getFlag())
                        throw new RuntimeException("Result pickling interrupted");
                }
            }
            out.write(Opcodes.LIST);

            if (params != null && includeAttrs)
                new ResultAttrsPickler(params, interruptedFlag).pickle(resultManager, out, pickler);
            else
                out.write(Opcodes.NONE);
        }
        out.write(Opcodes.TUPLE);
    }
}
