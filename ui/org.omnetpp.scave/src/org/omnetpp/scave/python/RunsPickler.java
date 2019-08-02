package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class RunsPickler implements IObjectPickler {

    String filterExpression;
    InterruptedFlag interruptedFlag;

    public RunsPickler(String filterExpression, InterruptedFlag interruptedFlag) {
        this.filterExpression = filterExpression;
        this.interruptedFlag = interruptedFlag;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;
        // in contrast to all other picklers, the list of run names is not wrapped in a tuple,
        // as it would only have one element, so there is no point to it.
        out.write(Opcodes.MARK);
        if (filterExpression != null && !filterExpression.trim().isEmpty()) {
            RunList runs = resultManager.getRuns();
            runs = resultManager.filterRunList(runs, filterExpression);

            if (ResultPicklingUtils.debug)
                Debug.println("pickling " + runs.size() + " runs");

            for (Run r : runs.toArray()) {
                pickler.save(r.getRunName());

                if (interruptedFlag.getFlag())
                    throw new RuntimeException("Run pickling interrupted");
            }
        }
        out.write(Opcodes.LIST);
    }
}
