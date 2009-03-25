package org.omnetpp.scave.writers.test;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.scave.writers.IOutputScalarManager;
import org.omnetpp.scave.writers.IOutputVector;
import org.omnetpp.scave.writers.IOutputVectorManager;
import org.omnetpp.scave.writers.impl.FileOutputScalarManager;
import org.omnetpp.scave.writers.impl.FileOutputVectorManager;

public class BasicTest {
    public static void main(String[] args) {
        String runID = FileOutputScalarManager.generateRunID("test1");
        IOutputScalarManager scalarManager = new FileOutputScalarManager("1.sca");
        scalarManager.open(runID, null);
        scalarManager.recordScalar("top.node1", "pk-sent", 632, null);
        scalarManager.recordScalar("top.node2", "pk-rcvd", 578, null);
        scalarManager.close();

        IOutputVectorManager vectorManager = new FileOutputVectorManager("1.vec");
        vectorManager.open(runID, null);

        List<IOutputVector> vectors = new ArrayList<IOutputVector>();
        for (int i=0; i<100000; i++) {
            if (Math.random() < 0.001)
                vectors.add(vectorManager.createVector("top.node"+(i%10), "eed-"+(int)(Math.random()*10), null));
            if (!vectors.isEmpty()) {
                int pos = (int)Math.floor(Math.random()*vectors.size());
                IOutputVector v = vectors.get(pos);
                v.record(i, 0.1001*i);
            }
        }
    }
}
