package com.swtworkbench.community.xswt;

import org.xmlpull.v1.XmlPullParser;

import com.swtworkbench.community.xswt.layoutbuilder.ILayoutBuilder;

public abstract interface ICustomNSHandler {
    public abstract void handleNamespace(XmlPullParser paramXmlPullParser, ILayoutBuilder paramILayoutBuilder);
}