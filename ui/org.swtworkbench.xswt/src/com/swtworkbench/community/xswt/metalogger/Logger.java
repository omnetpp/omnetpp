package com.swtworkbench.community.xswt.metalogger;

import com.swtworkbench.community.xswt.XswtPlugin;

public class Logger {
    private static ILogger logger = null;

    public static ILogger log() {
        if (logger == null) {
            logger = new EclipseLogger(XswtPlugin.getDefault());
        }
        return logger;
    }

    public static void setLogger(ILogger logger) {
        Logger.logger = logger;
    }
}