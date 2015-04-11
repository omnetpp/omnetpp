/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ui;

import org.eclipse.core.resources.IProject;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.pojo.ChannelElement;
import org.omnetpp.ned.model.pojo.ChannelInterfaceElement;
import org.omnetpp.ned.model.pojo.ConnectionGroupElement;
import org.omnetpp.ned.model.pojo.ConnectionsElement;
import org.omnetpp.ned.model.pojo.GateElement;
import org.omnetpp.ned.model.pojo.GatesElement;
import org.omnetpp.ned.model.pojo.ImportElement;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
import org.omnetpp.ned.model.pojo.NedElementTags;
import org.omnetpp.ned.model.pojo.ParamElement;
import org.omnetpp.ned.model.pojo.ParametersElement;
import org.omnetpp.ned.model.pojo.PropertyElement;
import org.omnetpp.ned.model.pojo.SimpleModuleElement;
import org.omnetpp.ned.model.pojo.SubmodulesElement;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * Label provider for INedElement tree node. Assumes unparsed expressions.
 *
 * @author rhornig
 */
public class NedModelLabelProvider extends LabelProvider {
    private static NedModelLabelProvider instance;

    private boolean prefixWithParent = false; // prefix names with their parent type

    public NedModelLabelProvider() {
    }

    public NedModelLabelProvider(boolean prefixWithParent) {
        this.prefixWithParent = prefixWithParent;
    }

    @Override
    public String getText(Object obj) {
        if (obj instanceof INedTypeInfo)
            obj = ((INedTypeInfo)obj).getNedElement();

        if (!(obj instanceof INedElement))
            return obj.toString();

        INedElement model = (INedElement)obj;
        String label = "???";

        if (model instanceof ImportElement) {
            ImportElement node = (ImportElement)model;
            label = "import "+node.getImportSpec();
        }
        else if (model instanceof ConnectionGroupElement) {
            label = StringUtils.substringBefore(getSourceWithoutComments(model), " {");;
        }
        else if (model instanceof PropertyElement) {
            label = parentPrefixFor(model) + StringUtils.strip(getSourceWithoutComments(model), "@;");
        }
        else if (model instanceof ParamElement) {
            ParamElement node = (ParamElement)model;
            String attType = node.getAttribute(ParamElement.ATT_TYPE);
            label = "".equals(attType) ? "" : attType+" ";
            label += parentPrefixFor(node) + node.getName();

            if (StringUtils.isNotEmpty(node.getValue())) {
                if (node.getIsDefault())
                    label += " = default("+node.getValue()+")";
                else
                    label += " = "+ node.getValue();
            }
            else if (node.getIsDefault()) {
                label += " = default";
            }

        }
        else if (model instanceof SimpleModuleElement) {
            SimpleModuleElement node = (SimpleModuleElement)model;
            label = parentPrefixFor(node) + node.getName() + " (simple module)";
        }
        else if (model instanceof CompoundModuleElementEx) {
            CompoundModuleElementEx node = (CompoundModuleElementEx)model;
            label = parentPrefixFor(node) + node.getName() + (node.isNetwork() ? " (network)" : " (compound module)");
        }
        else if (model instanceof SubmoduleElementEx) {
            SubmoduleElementEx node = (SubmoduleElementEx)model;
            label = parentPrefixFor(node) + node.getName() + bracketizeIfNotEmpty(node.getVectorSize()) + " : ";
            String likeType = node.getLikeType();
            if (likeType == null || "".equals(likeType))
                 label += node.getType();
            else
                label += "like "+node.getLikeType();
        }
        else if (model instanceof ModuleInterfaceElement) {
            ModuleInterfaceElement node = (ModuleInterfaceElement)model;
            label = parentPrefixFor(node) + node.getName()+" (module interface)";
        }
        else if (model instanceof ChannelInterfaceElement) {
            ChannelInterfaceElement node = (ChannelInterfaceElement)model;
            label = parentPrefixFor(node) + node.getName()+" (channel interface)";
        }
        else if (model instanceof ChannelElement) {
            ChannelElement node = (ChannelElement)model;
            label = parentPrefixFor(node) + node.getName() + " (channel)";
        }
        else if (model instanceof GateElement) {
            GateElement node = (GateElement)model;
            String attType = node.getAttribute(ParamElement.ATT_TYPE);
            label = "".equals(attType) ? "" : attType+" ";
            label += parentPrefixFor(node);
            label += node.getName();
            String vectorSizeInBrackets = bracketizeIfNotEmpty(node.getVectorSize());
            label += vectorSizeInBrackets.equals("") ? (node.getIsVector() ? "[]" : "") : vectorSizeInBrackets;
        }
        else if (model instanceof ConnectionElementEx) {
            // create a simplified representation (only the node is retained)
            INedElement dummyConn = model.dup();
            label = StringUtils.strip(dummyConn.getNedSource().trim(), ";");
        }
        else {
            label = model.getReadableTagName();
        }

        return label;
    }

    private String parentPrefixFor(INedElement node) {
        if (!prefixWithParent)
            return "";

        // produce "TypeName." or "TypeName.submoduleName." or "Typename.<conn>."
        String prefix = "";
        INedElement e = node.getParent();
        while (e != null) {
            if (e instanceof SubmoduleElementEx) {
                SubmoduleElementEx ee = (SubmoduleElementEx)e;
                if (StringUtils.isEmpty(ee.getVectorSize()))
                    prefix = ee.getName() + ".";
                else
                    prefix = ee.getName() + "[].";
            }
            else if (e instanceof ConnectionElementEx) {
                prefix = "<conn>.";
            }
            else if (e instanceof INedTypeElement) {
                INedTypeElement ee = (INedTypeElement)e;
                prefix = ee.getName() + "." + prefix;
                return prefix;
            }
            e = e.getParent();
        }
        return "";

    }

    private static String bracketizeIfNotEmpty(String attr) {
        return attr==null || attr.equals("") ? "" : "["+attr+"]";
    }

    private static String getSourceWithoutComments(INedElement element) {
        INedElement node = element.deepDup();
        // remove all comment nodes
        while(node.getFirstChildWithTag(NedElementTags.NED_COMMENT) != null)
            node.getFirstChildWithTag(NedElementTags.NED_COMMENT).removeFromParent();
        return StringUtils.stripToEmpty(StringUtils.substringBefore(node.getNedSource(), "\n"));
    }

    @Override
    public Image getImage(Object obj) {
        if (obj instanceof INedTypeInfo)
            obj = ((INedTypeInfo)obj).getNedElement();

        if (!(obj instanceof INedElement))
            return null;

        INedElement model = (INedElement)obj;
        Image image = null;
        if (model instanceof IHasDisplayString && !(model instanceof ChannelElementEx || model instanceof ChannelInterfaceElementEx)) {
            DisplayString dps = ((IHasDisplayString) model).getDisplayString();
            IProject project = model.getSelfOrEnclosingTypeElement().getNedTypeInfo().getProject();
            image = ImageFactory.of(project).getIconImage(dps.getAsString(IDisplayString.Prop.IMAGE));
        }

        if (image != null)
            return image;
        else if (model instanceof ImportElement) {
            image = ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_IMPORT);
        } else if (model instanceof PropertyElement) {
            image = ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_PROPERTY);
        } else if (model instanceof ParamElement) {
            image = ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_PARAM);
        } else if (model instanceof SimpleModuleElement) {
            image = ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_SIMPLEMODULE);
        } else if (model instanceof CompoundModuleElementEx) {
            image = ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_COMPOUNDMODULE);
        } else if (model instanceof SubmoduleElementEx) {
            image = ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_SUBMODULE);
        } else if (model instanceof ModuleInterfaceElement) {
            image = ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_INTERFACE);
        } else if (model instanceof ChannelInterfaceElement) {
            image = ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_CHANNELINTERFACE);
        } else if (model instanceof ChannelElement) {
            image = ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_CHANNEL);
        } else if (model instanceof GateElement) {
            image = ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_GATE);
        } else if (model instanceof ConnectionElementEx) {
            image = ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_CONNECTION);
        } else if (model instanceof TypesElement || model instanceof ParametersElement ||
                   model instanceof GatesElement || model instanceof SubmodulesElement ||
                   model instanceof ConnectionsElement || model instanceof ConnectionGroupElement) {
            image = ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_FOLDER);
        }

        return image;
    }

    /**
     * Returns the default label/icon provider for NED model trees
     */
    public static ILabelProvider getInstance() {
        if (instance == null)
            instance = new NedModelLabelProvider();
        return instance;
    }
}