package com.swtworkbench.community.xswt;

import com.swtworkbench.community.xswt.layoutbuilder.SWTSourceBuilder;
import com.swtworkbench.community.xswt.metalogger.ILogger;
import com.swtworkbench.community.xswt.metalogger.Logger;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.PrintStream;

public class XswtCC {
    public static void main(String[] args) throws IOException {
        try {
            SWTSourceBuilder source = new SWTSourceBuilder();
            XSWT.layoutBuilder = source;

            FileInputStream input = new FileInputStream(args[0]);
            XSWT.create(null, input);

            System.out.println(source);
        }
        catch (XSWTException e) {
            Logger.log().error(e, "Unable to parse XSWT file");
        }
        catch (IOException e) {
            Logger.log().error(e, "Unable to parse XSWT file");
        }
    }
}