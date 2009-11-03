package com.swtworkbench.community.xswt.metalogger;

import com.swtworkbench.community.xswt.Activator;

public class Logger {
    private static ILogger logger = null;

    public static ILogger log() {
        if (logger == null) {
            logger = new EclipseLogger(Activator.getDefault());
        }
        return logger;
    }

    public static void setLogger(ILogger logger) {
        Logger.logger = logger;
    }
}