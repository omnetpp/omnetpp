package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.scave.computed.XYVector;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.script.ScriptEngine;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class VectorResultsPickler implements IObjectPickler {

    static void pickleVectorResult(ResultFileManager resultManager, long ID, XYVector data, Pickler pickler,
            OutputStream out) throws PickleException, IOException {
        VectorResult result = resultManager.getVector(ID);

        out.write(Opcodes.MARK);
        {
            pickler.save(result.getRun().getRunName());
            pickler.save("vector");
            pickler.save(result.getModuleName());
            pickler.save(result.getName());
            pickler.save(null); // attrname
            pickler.save(null); // value

            ResultPicklingUtils.pickleDoubleArray(data.x, out);
            ResultPicklingUtils.pickleDoubleArray(data.y, out);
        }
        out.write(Opcodes.TUPLE);

        ResultPicklingUtils.pickleResultAttributes(result, pickler, out);
    }

    public static void pickleVectorsFiltered(ResultFileManager resultManager, String filter, Pickler pickler,
            OutputStream out) throws IOException {
        out.write(Opcodes.MARK);
        {
            IDList vectors = resultManager.getAllVectors();

            vectors = resultManager.filterIDList(vectors, filter);
            System.out.println("pickling " + vectors.size() + " vectors");

            ResultPicklingUtils.pickleRunsOfResults(resultManager, vectors, pickler, out);

            XYVector[] vectorsData = ScriptEngine.getDataOfVectors(resultManager, vectors, null);

            for (int i = 0; i < vectors.size(); ++i)
                pickleVectorResult(resultManager, vectors.get(i), vectorsData[i], pickler, out);
        }
        out.write(Opcodes.LIST);
    }

    String filter;

    public VectorResultsPickler(String filter) {
        this.filter = filter;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        pickleVectorsFiltered((ResultFileManager) obj, filter, pickler, out);
    }
}
