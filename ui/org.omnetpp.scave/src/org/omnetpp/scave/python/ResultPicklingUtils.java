package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;

import org.omnetpp.common.engine.ILock;
import org.omnetpp.scave.engine.ResultFileManager;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class ResultPicklingUtils {

    public static double[] bytesToDoubles(byte[] bytes) {
        ByteBuffer byteBuffer = ByteBuffer.wrap(bytes);
        double[] doubles = new double[bytes.length / 8];
        byteBuffer.asDoubleBuffer().get(doubles);
        return doubles;
    }

    public static byte[] doublesToBytes(double[] doubles) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(doubles.length * 8);
        byteBuffer.asDoubleBuffer().put(doubles);
        return byteBuffer.array();
    }

    public static void pickleDoubleArray(double[] array, OutputStream out) throws IOException {
        // number of bytes
        int s = array.length * 8;

        out.write(Opcodes.BINBYTES);
        // 4 bytes length prefix
        out.write(s >> 0);
        out.write(s >> 8);
        out.write(s >> 16);
        out.write(s >> 24);
        // actual data
        out.write(doublesToBytes(array));
    }

    public static byte[] pickleResultsUsing(ResultFileManager resultManager, IObjectPickler resultsPickler) throws PickleException, IOException {
        ILock l = resultManager.getReadLock();
        l.lock();
        Pickler p = new Pickler(true);
        Pickler.registerCustomPickler(ResultFileManager.class, resultsPickler);
        byte[] pickle = p.dumps(resultManager);
        l.unlock();
        return pickle;
    }
}
