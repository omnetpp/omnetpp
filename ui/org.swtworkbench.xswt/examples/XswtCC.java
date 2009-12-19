package examples;

import com.swtworkbench.community.xswt.XSWT;
import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.layoutbuilder.SWTSourceBuilder;
import com.swtworkbench.community.xswt.metalogger.ILogger;
import com.swtworkbench.community.xswt.metalogger.Logger;
import java.io.IOException;
import java.io.PrintStream;

public class XswtCC
{
  public static void main(String[] args)
    throws IOException
  {
    XswtCC klass = new XswtCC();
    try
    {
      SWTSourceBuilder source = new SWTSourceBuilder();
      XSWT.layoutBuilder = source;

      XSWT.create(null, klass.getClass().getResourceAsStream(
        "demo.xswt"));

      System.out.println(source);
    }
    catch (XSWTException e) {
      Logger.log().error(e, "Unable to parse XSWT file");
    }
  }
}