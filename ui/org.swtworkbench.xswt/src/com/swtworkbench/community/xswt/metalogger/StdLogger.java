package com.swtworkbench.community.xswt.metalogger;

public class StdLogger extends AbstractLogger {
    public void error(Throwable t, String message) {
        System.err.println(message);
        t.printStackTrace();
    }

    public void message(String message) {
        System.out.println(message);
    }

    public void debug(Class subject, String message) {
        if (!(isDebug(subject)))
            return;
        System.out.println(message);
    }
}