package org.omnetpp.ned.model.ui;

import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.pojo.*;

/**
 * Label provider for INEDElement tree node. Assumes unparsed expressions.
 *
 * @author rhornig
 */
public class NedModelLabelProvider extends LabelProvider {

	@Override
    public String getText(Object obj) {
	    if (obj instanceof INEDTypeInfo)
	        obj = ((INEDTypeInfo)obj).getNEDElement();
	    
        INEDElement model = (INEDElement)obj;
        String label = "???";

        if (model instanceof ImportElement) {
            ImportElement node = (ImportElement)model;
            label = "import "+node.getImportSpec();
        }
        else if (model instanceof ConnectionGroupElement) {
            label = StringUtils.substringBefore(getSourceWithoutComments(model), " {");;
        }
        else if (model instanceof PropertyElement) {
            label = StringUtils.strip(getSourceWithoutComments(model), "@;");
        }
        else if (model instanceof ParamElement) {
            ParamElement node = (ParamElement)model;
            String attType = node.getAttribute(ParamElement.ATT_TYPE);
            label = "".equals(attType) ? "" : attType+" ";
            label += node.getName();

            if (StringUtils.isNotEmpty(node.getValue())) {
                if (node.getIsDefault() )
                    label += " = default("+node.getValue()+")";
                else
                    label += " = "+ node.getValue();
            }

        }
        else if (model instanceof SimpleModuleElement) {
            SimpleModuleElement node = (SimpleModuleElement)model;
            label = "simple "+node.getName();
        }
        else if (model instanceof CompoundModuleElementEx) {
            CompoundModuleElementEx node = (CompoundModuleElementEx)model;
            label = (node.isNetwork() ? "network " : "module ")+node.getName();
        }
        else if (model instanceof SubmoduleElementEx) {
            SubmoduleElementEx node = (SubmoduleElementEx)model;
            label = node.getName()+bracketizeIfNotEmpty(node.getVectorSize())+" : ";
            String likeType = node.getLikeType();
            if (likeType == null || "".equals(likeType))
                 label += node.getType();
            else
                label += "like "+node.getLikeType();
        }
        else if (model instanceof ModuleInterfaceElement) {
            ModuleInterfaceElement node = (ModuleInterfaceElement)model;
            label = node.getName()+" (module interface)";
        }
        else if (model instanceof ChannelInterfaceElement) {
            ChannelInterfaceElement node = (ChannelInterfaceElement)model;
            label = node.getName()+" (channel interface)";
        }
        else if (model instanceof ChannelSpecElement) {
            ChannelSpecElement node = (ChannelSpecElement)model;
            label = "channel ";
            String likeType = node.getLikeType();
            if (likeType == null || "".equals(likeType))
                 label += node.getType();
            else
                label += "like "+node.getLikeType();
        }
        else if (model instanceof ChannelElement) {
            ChannelElement node = (ChannelElement)model;
            label = "channel "+node.getName();
        }
        else if (model instanceof GateElement) {
            GateElement node = (GateElement)model;
            String attType = node.getAttribute(ParamElement.ATT_TYPE);
            label = "".equals(attType) ? "" : attType+" ";
            label += node.getName();
            String vectorSizeInBrackets = bracketizeIfNotEmpty(node.getVectorSize());
            label += vectorSizeInBrackets.equals("") ? (node.getIsVector() ? "[]" : "") : vectorSizeInBrackets;
        }
        else if (model instanceof ConnectionElementEx) {
            // create a simplified representation (only the node and its channel spec is retained)
            INEDElement dummyConn = model.dup();
            INEDElement dummyChSpec = ((ConnectionElementEx)model).getFirstChannelSpecChild();
            if (dummyChSpec != null)
                dummyConn.appendChild(dummyChSpec.dup());

            label = StringUtils.strip(dummyConn.getNEDSource().trim(), ";");
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
        if (model instanceof IHasDisplayString && !(model instanceof ChannelElementEx || model instanceof ChannelInterfaceElementEx)) {
            DisplayString dps = ((IHasDisplayString) model).getDisplayString();
            image = ImageFactory.getIconImage(dps.getAsString(IDisplayString.Prop.IMAGE));
        }

        if (image != null)
            return image;
        else if (model instanceof ImportElement) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_IMPORT);
        } else if (model instanceof PropertyElement) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_PROPERTY);
        } else if (model instanceof ParamElement) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_PARAM);
        } else if (model instanceof SimpleModuleElement) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_SIMPLEMODULE);
        } else if (model instanceof CompoundModuleElementEx) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_COMPOUNDMODULE);
        } else if (model instanceof SubmoduleElementEx) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_SUBMODULE);
        } else if (model instanceof ModuleInterfaceElement) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_INTERFACE);
        } else if (model instanceof ChannelInterfaceElement) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNELINTERFACE);
        } else if (model instanceof ChannelSpecElement) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNEL);
        } else if (model instanceof ChannelElement) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNEL);
        } else if (model instanceof GateElement) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_GATE);
        } else if (model instanceof ConnectionElementEx) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CONNECTION);
        } else if (model instanceof TypesElement || model instanceof ParametersElement ||
                   model instanceof GatesElement || model instanceof SubmodulesElement ||
                   model instanceof ConnectionsElement || model instanceof ConnectionGroupElement) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_FOLDER);
        }

        return image;
	}
}