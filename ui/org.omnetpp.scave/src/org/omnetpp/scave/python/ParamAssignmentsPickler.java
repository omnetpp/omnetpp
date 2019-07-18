package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;
import java.util.HashSet;
import java.util.List;

import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.OrderedKeyValueList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.StringPair;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class ParamAssignmentsPickler implements IObjectPickler {

    String filterExpression;
    List<String> runIDs;
    InterruptedFlag interruptedFlag;

    public ParamAssignmentsPickler(String filterExpression, InterruptedFlag interruptedFlag) {
        this.filterExpression = filterExpression;
        this.interruptedFlag = interruptedFlag;
    }

    public ParamAssignmentsPickler(List<String> runIDs, InterruptedFlag interruptedFlag) {
        this.runIDs = runIDs;
        this.interruptedFlag = interruptedFlag;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;

        out.write(Opcodes.MARK);
        if (filterExpression != null && !filterExpression.trim().isEmpty()) {

            OrderedKeyValueList patterns = resultManager.getMatchingParamAssignments(filterExpression);

            for (int i = 0; i < patterns.size(); ++i) {
                StringPair ri = patterns.get(i);

                Run run = resultManager.getRunByName(ri.getFirst());
                String paName = ri.getSecond();

                out.write(Opcodes.MARK);
                {
                    pickler.save(run.getRunName());
                    pickler.save(paName);
                    pickler.save(run.getParamAssignment(paName));
                }
                out.write(Opcodes.TUPLE);

                if (i % 10 == 0 && interruptedFlag.getFlag())
                    throw new RuntimeException("Param assignment pickling interrupted");
            }
        }
        else {
            HashSet<String> runsSet = new HashSet<String>();
            runsSet.addAll(runIDs);

            RunList allRuns = resultManager.getRuns();
            for (int i = 0; i < allRuns.size(); ++i) {
                Run r = allRuns.get(i);
                if (runsSet.contains(r.getRunName())) {
                    OrderedKeyValueList params = r.getParamAssignments();
                    for (int j = 0; j < params.size(); ++j) {
                        String key = params.get(j).getFirst();
                        String value = params.get(j).getSecond();

                        out.write(Opcodes.MARK);
                        {
                            pickler.save(r.getRunName());
                            pickler.save(key);
                            pickler.save(value);
                        }
                        out.write(Opcodes.TUPLE);

                        if (i % 10 == 0 && interruptedFlag.getFlag())
                            throw new RuntimeException("Param assignment pickling interrupted");
                    }
                }
            }
        }
        out.write(Opcodes.LIST);
    }
}
