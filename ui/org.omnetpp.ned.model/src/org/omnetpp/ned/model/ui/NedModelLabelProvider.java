package org.omnetpp.ned.model.ui;

import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.ConnectionNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.pojo.*;

/**
 * Label provider for INEDElement tree node. Assumes unparsed expressions.
 *
 * @author rhornig
 */
public class NedModelLabelProvider extends LabelProvider {

	@Override
    public String getText(Object obj) {
        INEDElement model = (INEDElement)obj;
        String label = "???";

        if (model instanceof ImportNode) {
            ImportNode node = (ImportNode)model;
            label = "import \""+node.getFilename()+"\"";
        }
        else if (model instanceof ConnectionGroupNode) {
            label = StringUtils.substringBefore(getSourceWithoutComments(model), " {");;
        }
        else if (model instanceof PropertyNode) {
            label = StringUtils.strip(getSourceWithoutComments(model), "@;");
        }
        else if (model instanceof ParamNode) {
            ParamNode node = (ParamNode)model;
            String attType = node.getAttribute(ParamNode.ATT_TYPE);
            label = "".equals(attType) ? "" : attType+" ";
            label += node.getName();

            if (StringUtils.isNotEmpty(node.getValue())) {
                if (node.getIsDefault() )
                    label += " = default("+node.getValue()+")";
                else
                    label += " = "+ node.getValue();
            }

        }
        else if (model instanceof SimpleModuleNode) {
            SimpleModuleNode node = (SimpleModuleNode)model;
            label = "simple "+node.getName();
        }
        else if (model instanceof CompoundModuleNodeEx) {
            CompoundModuleNodeEx node = (CompoundModuleNodeEx)model;
            label = (node.getIsNetwork() ? "network " : "module ")+node.getName();
        }
        else if (model instanceof SubmoduleNodeEx) {
            SubmoduleNodeEx node = (SubmoduleNodeEx)model;
            label = node.getName()+bracketizeIfNotEmpty(node.getVectorSize())+" : ";
            String likeType = node.getLikeType();
            if (likeType == null || "".equals(likeType))
                 label += node.getType();
            else
                label += "like "+node.getLikeType();
        }
        else if (model instanceof ModuleInterfaceNode) {
            ModuleInterfaceNode node = (ModuleInterfaceNode)model;
            label = "interface " + node.getName();
        }
        else if (model instanceof ChannelInterfaceNode) {
            ChannelInterfaceNode node = (ChannelInterfaceNode)model;
            label = "channelinterface "+node.getName();
        }
        else if (model instanceof ChannelSpecNode) {
            ChannelSpecNode node = (ChannelSpecNode)model;
            label = "channel "+node.getType();
        }
        else if (model instanceof ChannelNode) {
            ChannelNode node = (ChannelNode)model;
            label = "channel "+node.getName();
        }
        else if (model instanceof GateNode) {
            GateNode node = (GateNode)model;
            String attType = node.getAttribute(ParamNode.ATT_TYPE);
            label = "".equals(attType) ? "" : attType+" ";
            label += node.getName();
            String vectorSizeInBrackets = bracketizeIfNotEmpty(node.getVectorSize());
            label += vectorSizeInBrackets.equals("") ? (node.getIsVector() ? "[]" : "") : vectorSizeInBrackets;
        }
        else if (model instanceof ConnectionNodeEx) {
            label = StringUtils.strip(getSourceWithoutComments(model), ";");
        }
        else if (model != null){
            label = model.getReadableTagName();
        }

        return label;
	}

	private static String bracketizeIfNotEmpty(String attr) {
		return attr==null || attr.equals("") ? "" : "["+attr+"]";
	}

    private static String getSourceWithoutComments(INEDElement element) {
        INEDElement node = element.deepDup();
        // remove all comment nodes
        while(node.getFirstChildWithTag(NEDElementTags.NED_COMMENT) != null)
            node.getFirstChildWithTag(NEDElementTags.NED_COMMENT).removeFromParent();
        return StringUtils.stripToEmpty(StringUtils.substringBefore(node.getNEDSource(), "\n"));
    }

    @Override
    public Image getImage(Object obj) {
        INEDElement model = (INEDElement)obj;
        Image image = null;
        if (model instanceof IHasDisplayString) {
            DisplayString dps = ((IHasDisplayString) model).getDisplayString();
            image = ImageFactory.getIconImage(dps.getAsString(IDisplayString.Prop.IMAGE));
        }

        if (image != null)
            return image;
        else if (model instanceof ImportNode) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_IMPORT);
        } else if (model instanceof PropertyNode) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_PROPERTY);
        } else if (model instanceof ParamNode) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_PARAM);
        } else if (model instanceof SimpleModuleNode) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_SIMPLEMODULE);
        } else if (model instanceof CompoundModuleNodeEx) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_COMPOUNDMODULE);
        } else if (model instanceof SubmoduleNodeEx) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_SUBMODULE);
        } else if (model instanceof ModuleInterfaceNode) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_INTERFACE);
        } else if (model instanceof ChannelInterfaceNode) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNELINTERFACE);
        } else if (model instanceof ChannelSpecNode) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNEL);
        } else if (model instanceof ChannelNode) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNEL);
        } else if (model instanceof GateNode) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_GATE);
        } else if (model instanceof ConnectionNodeEx) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CONNECTION);
        } else if (model instanceof TypesNode || model instanceof ParametersNode ||
                   model instanceof GatesNode || model instanceof SubmodulesNode ||
                   model instanceof ConnectionsNode || model instanceof ConnectionGroupNode) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_FOLDER);
        }

        return image;
	}
}