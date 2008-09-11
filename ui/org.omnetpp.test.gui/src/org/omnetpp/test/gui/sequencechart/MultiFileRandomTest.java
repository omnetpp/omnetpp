package org.omnetpp.test.gui.sequencechart;

import junit.framework.TestCase;

public class MultiFileRandomTest extends TestCase {
    public void testRandom() {
        for (String fileName : new String[] {
                "_empty.log", "_one-event.log", "_two-events.log",
                "Tictoc1-0.log", "Tictoc9-0.log", "Tictoc-Killer.log",
                "Fifo1-0.log", "Ping1-0.log", "Dyna-0.log",
                "PureAloha2-0.log", "SlottedAloha2-0.log"})
            doTest("results/" + fileName);
    }
    
    private void doTest(String fileName) {
        new RandomTest(fileName, 10 * 1000, false).testRandom();
    }
}
