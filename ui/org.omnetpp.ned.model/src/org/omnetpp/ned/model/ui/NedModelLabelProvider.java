/**
 * 
 */
package org.omnetpp.ned.model.ui;

import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IHasDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDElementUtil;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.ConnectionNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned.model.pojo.ChannelNode;
import org.omnetpp.ned.model.pojo.ChannelSpecNode;
import org.omnetpp.ned.model.pojo.ConnectionsNode;
import org.omnetpp.ned.model.pojo.GateNode;
import org.omnetpp.ned.model.pojo.GatesNode;
import org.omnetpp.ned.model.pojo.ImportNode;
import org.omnetpp.ned.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.ParametersNode;
import org.omnetpp.ned.model.pojo.PropertyNode;
import org.omnetpp.ned.model.pojo.SimpleModuleNode;
import org.omnetpp.ned.model.pojo.SubmodulesNode;
import org.omnetpp.ned.model.pojo.TypesNode;

/**
 * Label provider for NEDElement tree node. Assumes unparsed expressions.
 * 
 * @author rhornig
 */
public class NedModelLabelProvider extends LabelProvider {

	public String getText(Object obj) {
        NEDElement model = (NEDElement)obj;
        String label = "???";
        
        if (model instanceof ImportNode) {
            ImportNode node = (ImportNode)model;
            label = "import \""+node.getFilename()+"\"";
        } 
        else if (model instanceof PropertyNode) {
            PropertyNode node = (PropertyNode)model;
            label = "@"+node.getName(); //FIXME TODO: produce value by visiting children
        } 
        else if (model instanceof ParamNode) {
            ParamNode node = (ParamNode)model;
            String attType = node.getAttribute(ParamNode.ATT_TYPE);
            label = "".equals(attType) ? "" : attType+" ";
            label += node.getName();
        } 
        else if (model instanceof SimpleModuleNode) {
            SimpleModuleNode node = (SimpleModuleNode)model;
            label = "simple "+node.getName();
        } 
        else if (model instanceof CompoundModuleNodeEx) {
            CompoundModuleNodeEx node = (CompoundModuleNodeEx)model;           
            label = ((node.getIsNetwork()) ? "network " : "module ")+node.getName();
        }
        else if (model instanceof SubmoduleNodeEx) {
            SubmoduleNodeEx node = (SubmoduleNodeEx)model;
            label = node.getName()+bracketizeIfNotEmpty(node.getVectorSize())+" : ";
            label += node.getType();  //FIXME "like" !!!
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
            ConnectionNodeEx node = (ConnectionNodeEx)model;
            StringBuffer srclabel = new StringBuffer();
            if (!"".equals(node.getSrcModule())) {
                srclabel.append(node.getSrcModule());
                srclabel.append(bracketizeIfNotEmpty(node.getSrcModuleIndex()));
                srclabel.append(".");
            }
            srclabel.append(node.getSrcGate());
            srclabel.append(bracketizeIfNotEmpty(node.getSrcGateIndex()));
            
            StringBuffer destlabel = new StringBuffer();
            if(!"".equals(node.getDestModule())) {
                destlabel.append(node.getDestModule());
                destlabel.append(bracketizeIfNotEmpty(node.getDestModuleIndex()));
                destlabel.append(".");
            }
            destlabel.append(node.getDestGate());
            destlabel.append(bracketizeIfNotEmpty(node.getDestGateIndex()));
            switch (node.getArrowDirection()) {
                case NEDElementUtil.NED_ARROWDIR_L2R :
                    label = srclabel + " --> " + destlabel;
                    break;
                case NEDElementUtil.NED_ARROWDIR_R2L :
                    label = destlabel + " <-- " + srclabel;
                    break;
                default :
                    label = srclabel + " <--> " + destlabel;
                break;
            }
        } 
        else {
            label = model.getTagName();
        }
        
        return label;
	}

	private static String bracketizeIfNotEmpty(String attr) {
		return (attr==null || attr.equals("")) ? "" : "["+attr+"]";
	}
	
	public Image getImage(Object obj) {
        NEDElement model = (NEDElement)obj;
        Image image = null;
        if (model instanceof IHasDisplayString) {
            DisplayString dps = ((IHasDisplayString) model).getEffectiveDisplayString();
            image = ImageFactory.getIconImage(dps.getAsStringDef(IDisplayString.Prop.IMAGE));
        }

        if (image != null) {
            return image;
        } else if (model instanceof ImportNode) {
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
                   model instanceof ConnectionsNode) {
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_FOLDER);
        }
        
        return image;
	}
}