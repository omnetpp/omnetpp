package org.omnetpp.scave.python;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;

import org.omnetpp.common.Debug;
import org.omnetpp.common.engine.ILock;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScaveEngine;
import org.omnetpp.scave.engine.XYArray;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.PickleUtils;
import net.razorvine.pickle.Pickler;

public class ResultPicklingUtils {

    public static boolean debug = Debug.isChannelEnabled("pickling");

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

    static int shmSerial = 0;

    private static void writeString(String s, OutputStream out) throws IOException {
        byte[] encodedX = s.getBytes("UTF-8");
        out.write(Opcodes.BINUNICODE);
        out.write(PickleUtils.integer_to_bytes(encodedX.length));
        out.write(encodedX);
    }

    public static void pickleXYArray(XYArray array, OutputStream out) throws IOException {
        int l = array.length();
        // number of bytes
        long s = l * 8;

        if (s == 0) {
            writeString("<EMPTY> 0", out);
            writeString("<EMPTY> 0", out);
            return;
        }

        String nameX = "/vectordata-" + System.nanoTime() + "-" + shmSerial + "-x";
        String nameY = "/vectordata-" + System.nanoTime() + "-" + shmSerial + "-y";
        shmSerial += 1;

        // X
        writeString(nameX + " " + s, out);
        ScaveEngine.createSharedMemory(nameX, s);

        ByteBuffer shmX = ScaveEngine.mapSharedMemory(nameX, s);

        // actual data
        for (int i = 0; i < l; ++i)
            shmX.putDouble(array.getX(i));

        ScaveEngine.unmapSharedMemory(shmX);
        // Y
        writeString(nameY + " " + s, out);

        ScaveEngine.createSharedMemory(nameY, s);
        ByteBuffer shmY = ScaveEngine.mapSharedMemory(nameY, s);

        // actual data
        for (int i = 0; i < l; ++i)
            shmY.putDouble(array.getY(i));

        ScaveEngine.unmapSharedMemory(shmY);
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
        return ResultFileManager.callWithReadLock(resultManager, () -> {
            Pickler p = new Pickler(true);
            Pickler.registerCustomPickler(ResultFileManager.class, resultsPickler);

            // We need to limit the size of the pickle, because:
            // Py4J will Base64 encode it (1.25x) into a UTF-16 String (x2), then
            // it will be sent to the Python process (x2), and depickled (+1), so overall,
            // the used memory is at least 1 + 1.25*2*2 + 1 = 7x the size of the pickle we produce.
            // Note that vector data is not pickled (we use shared memory for that).
            long SIZE_LIMIT = ScaveEngine.getAvailableMemoryBytes() / 10;

            ByteArrayOutputStream bo = new ByteArrayOutputStream() {

                protected void checkSizeLimit() {
                    if (buf.length > SIZE_LIMIT)
                        throw new RuntimeException("Pickle size limit exceeded.");
                }

                @Override
                public synchronized void write(byte[] b, int off, int len) {
                    super.write(b, off, len);
                    checkSizeLimit();
                }

                @Override
                public synchronized void write(int b) {
                    super.write(b);
                    checkSizeLimit();
                }
            };

            p.dump(resultManager, bo);
            bo.flush();

            byte[] pickle = bo.toByteArray();

            if (debug)
                Debug.println("Pickle size: " + pickle.length);
            return pickle;
        });
    }
}
