package com.swtworkbench.community.xswt.metalogger;

public class Logger {
    private static ILogger logger = null;

    public static ILogger log() {
        if (logger == null) {
            logger = new StdLogger();
        }
        return logger;
    }

    public static void setLogger(ILogger logger) {
        logger = logger;
    }
}