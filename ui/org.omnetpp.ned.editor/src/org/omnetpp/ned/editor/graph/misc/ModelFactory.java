/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.requests.CreationFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.NedElementFactoryEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.NedElementTags;


/**
 * A factory used to create new model elements corresponding to graphical editor elements.
 *
 * @author rhornig
 */
public class ModelFactory implements CreationFactory {
    protected int tagCode;
    protected String name;
    protected String type;
    protected boolean useLike = false;
    protected String bannerComment;

    /**
     * @param objectType The class identifier of the INedElement passed NedElementFactoryEx.getInstance().createElement
     */
    public ModelFactory(int tagCode) {
        this(tagCode, null);
    }
    /**
     * @param objectType The class identifier of the INedElement
     * @param name The optional name of the new element
     */
    public ModelFactory(int tagCode, String name) {
        this(tagCode, name, null, false);
    }

    /**
     * @param objectType The class identifier of the INedElement
     * @param name The optional name of the new element
     * @param type The optional type of the new element (for submodules and connections)
     */
    public ModelFactory(int tagCode, String name, String type) {
        this(tagCode, name, type, false);
    }

    /**
     * @param objectType The class identifier of the INedElement
     * @param name The optional name of the new element
     * @param type The optional type of the new element (for submodules and connections)
     * @param useLike The provided type will be used as an interface type t
     */
    public ModelFactory(int tagCode, String name, String type, boolean useLike) {
        this.tagCode = tagCode;
        this.name = name;
        this.type = type;
        this.useLike = useLike;
    }

    /**
     * Set the banner commend added to the element. Set it to "" or null to avoid comment creation.
     */
    void setBannerComment(String comment) {
        bannerComment = comment;
    }

    public Object getNewObject() {
        INedTypeResolver resolver = NedResourcesPlugin.getNedResources();
        INedElement element = NedElementFactoryEx.getInstance().createElement(resolver, tagCode);

        if (!StringUtils.isEmpty(bannerComment)) {
            // add trailing empty lines
            CommentElement ceTrail = (CommentElement)NedElementFactoryEx.getInstance().createElement(NedElementTags.NED_COMMENT);
            ceTrail.setLocid("trailing");
            ceTrail.setContent("\n\n");
            element.insertChildBefore(element.getFirstChild(), ceTrail);
            CommentElement ce = (CommentElement)NedElementFactoryEx.getInstance().createElement(NedElementTags.NED_COMMENT);
            ce.setLocid("banner");
            ce.setContent(bannerComment);
            element.insertChildBefore(ceTrail, ce);
        }

        if (element instanceof IHasName)
            ((IHasName)element).setName(name);

        if (element instanceof ISubmoduleOrConnection) {
            if (!useLike)
                ((ISubmoduleOrConnection)element).setType(type);
            else {
                ((ISubmoduleOrConnection)element).setLikeType(type);
                // TODO maybe we could ask the user to specify the param name in a dialog box
                ((ISubmoduleOrConnection)element).setLikeExpr("paramName");
            }
        }

        return element;
    }

    public Object getObjectType() {
        return tagCode;
    }

}

