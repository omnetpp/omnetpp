package com.swtworkbench.community.xswt;

import com.swtworkbench.community.xswt.layoutbuilder.ILayoutBuilder;
import org.xmlpull.v1.XmlPullParser;

public abstract interface ICustomNSHandler {
    public abstract void handleNamespace(XmlPullParser paramXmlPullParser, ILayoutBuilder paramILayoutBuilder);
}