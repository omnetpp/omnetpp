package org.omnetpp.common.wizard;

import org.omnetpp.common.CommonPlugin;

import freemarker.log.Logger;
import freemarker.log.LoggerFactory;

/**
 * Eclipse logger for Freemarker. Needed because the logger chosen by Freemarker
 * by default (JDK14Logger) also prints to System.out which we don't want.
 *
 * @author Andras
 */
public class FreemarkerEclipseLoggerFactory implements LoggerFactory
{
    public Logger getLogger(String category)
    {
        return new EclipseLogger();
    }

    private static class EclipseLogger extends Logger
    {
        public void error(String message)
        {
            CommonPlugin.logError(message, null);
        }

        public void error(String message, Throwable t)
        {
            CommonPlugin.logError(message, t);
        }

        public void debug(String message)
        {
        }

        public void debug(String message, Throwable t)
        {
        }

        public void info(String message)
        {
        }

        public void info(String message, Throwable t)
        {
        }

        public void warn(String message)
        {
        }

        public void warn(String message, Throwable t)
        {
        }

        public boolean isDebugEnabled()
        {
            return false;
        }

        public boolean isInfoEnabled()
        {
            return false;
        }

        public boolean isWarnEnabled()
        {
            return true;
        }

        public boolean isErrorEnabled()
        {
            return true;
        }

        public boolean isFatalEnabled()
        {
            return true;
        }
    }
}
