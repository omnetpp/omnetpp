package org.omnetpp.common;

import org.apache.commons.lang3.StringUtils;

public class OmnetppUtils {

    /*
     * Returns whether the IDE was started from an opp_env shell.
    */
    static public boolean isInsideOppEnv() {
        return !StringUtils.isEmpty(System.getenv("OPP_ENV_VERSION"));
    }
}
