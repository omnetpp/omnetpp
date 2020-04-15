package com.swtworkbench.community.xswt.test;

import java.io.IOException;

import com.swtworkbench.community.xswt.XSWT;
import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.layoutbuilder.SWTSourceBuilder;
import com.swtworkbench.community.xswt.metalogger.Logger;

public class TestCodeGen {
    public static void main(String[] args) throws IOException {
        try {
            SWTSourceBuilder translator = new SWTSourceBuilder();
            XSWT.layoutBuilder = translator;

            XSWT.create(null, XSWTTest.class.getResource("SimpleSample.xswt").openStream());

            System.out.println(translator);
        }
        catch (XSWTException e) {
            Logger.log().error(e, "Unable to parse XSWT file");
        }
        catch (IOException e) {
            Logger.log().error(e, "Unable to parse XSWT file");
        }
    }
}