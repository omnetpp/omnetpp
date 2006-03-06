package org.omnetpp.ned.editor.graph.edit;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.EditPolicy;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedTreeEditPolicy;
import org.omnetpp.ned.editor.graph.misc.ImageFactory;
import org.omnetpp.ned.editor.graph.model.CompoundModuleNodeEx;
import org.omnetpp.ned.editor.graph.model.ConnectionNodeEx;
import org.omnetpp.ned.editor.graph.model.INedModelElement;
import org.omnetpp.ned.editor.graph.model.INEDChangeListener;
import org.omnetpp.ned.editor.graph.model.NedElementExUtil;
import org.omnetpp.ned.editor.graph.model.SubmoduleNodeEx;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned2.model.pojo.ChannelNode;
import org.omnetpp.ned2.model.pojo.ChannelSpecNode;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.ConnectionGroupNode;
import org.omnetpp.ned2.model.pojo.ConnectionNode;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;
import org.omnetpp.ned2.model.pojo.GateGroupNode;
import org.omnetpp.ned2.model.pojo.GateNode;
import org.omnetpp.ned2.model.pojo.GatesNode;
import org.omnetpp.ned2.model.pojo.ImportNode;
import org.omnetpp.ned2.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned2.model.pojo.NedFileNode;
import org.omnetpp.ned2.model.pojo.ParamGroupNode;
import org.omnetpp.ned2.model.pojo.ParamNode;
import org.omnetpp.ned2.model.pojo.ParametersNode;
import org.omnetpp.ned2.model.pojo.PropertyNode;
import org.omnetpp.ned2.model.pojo.SimpleModuleNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;
import org.omnetpp.ned2.model.pojo.SubmodulesNode;
import org.omnetpp.ned2.model.pojo.TypesNode;

/**
 * EditPart for Logic components in the Tree.
 */
public class NedTreeEditPart extends org.eclipse.gef.editparts.AbstractTreeEditPart implements
        INEDChangeListener {

    /**
     * Constructor initializes this with the given model.
     * 
     * @param model
     *            Model for this.
     */
    public NedTreeEditPart(Object model) {
        super(model);
    }

    public void activate() {
        super.activate();
        ((INedModelElement)getModel()).addListener(this);
    }

    /**
     * Creates and installs pertinent EditPolicies for this.
     */
    protected void createEditPolicies() {
        installEditPolicy(EditPolicy.COMPONENT_ROLE, new NedComponentEditPolicy());
        installEditPolicy(EditPolicy.PRIMARY_DRAG_ROLE, new NedTreeEditPolicy());
    }

    public void deactivate() {
        ((INedModelElement)getModel()).removeListener(this);
        super.deactivate();
    }

    /**
     * Returns the children of this from the model, as this is capable enough of
     * holding EditParts.
     * 
     * @return List of children.
     */
    protected List getModelChildren() {
        List result = new ArrayList();
        NEDElement currElem = ((NEDElement)getModel());
        
        // if this is a channel spec we will give back the parameters subnode's children
        if (currElem instanceof ChannelSpecNode)
        	currElem = ((ChannelSpecNode)currElem).getFirstParametersChild();
        if(currElem == null)
        	return result;
        
        
        for(NEDElement child : currElem) {
        	// display only the following classes
        	if ((child instanceof NedFileNode) ||
        			(child instanceof ImportNode) ||
        			(child instanceof ChannelNode) ||
        			(child instanceof ChannelInterfaceNode) ||
        			(child instanceof SimpleModuleNode) ||
        			(child instanceof CompoundModuleNode) ||
        			(child instanceof ModuleInterfaceNode) ||
        			(child instanceof GatesNode) ||
        			(child instanceof ParametersNode) ||
        			(child instanceof ConnectionsNode) ||
        			(child instanceof SubmodulesNode) ||
        			(child instanceof SubmoduleNode) ||
        			(child instanceof GateNode) ||
        			(child instanceof GateGroupNode) ||
        			(child instanceof ParamNode) ||
        			(child instanceof ParamGroupNode) ||
        			(child instanceof PropertyNode) ||
        			(child instanceof ConnectionNode) ||
        			(child instanceof ConnectionGroupNode) ||
        			(child instanceof TypesNode) ||
        			(child instanceof ChannelSpecNode)) 
        		result.add(child);
        }
        return result;
    }

    /**
	 * Refreshes the visual properties of the TreeItem for this part.
     */
    @Override
    protected void refreshVisuals() {
        if (getWidget() instanceof Tree) return;
        // TODO add name to the label too
        NEDElement model = (NEDElement)getModel();
        String label = "???";
        Image image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_INVALID);
        
        if (model instanceof ImportNode) {
			ImportNode node = (ImportNode)model;
			label = "import \""+node.getFilename()+"\"";
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_IMPORT);
		} 
        else if (model instanceof PropertyNode) {
        	PropertyNode node = (PropertyNode)model;
			label = "@"+node.getName();
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_PROPERTY);
		} 
		else if (model instanceof ParamNode) {
        	ParamNode node = (ParamNode)model;
        	String attType = node.getAttribute(ParamNode.ATT_TYPE);
			label = "".equals(attType) ? "" : attType+" ";
			label += node.getName();
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_PARAM);
		} 
		else if (model instanceof SimpleModuleNode) {
        	SimpleModuleNode node = (SimpleModuleNode)model;
			label = "simple "+node.getName();
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_SIMPLEMODULE);
		} 
		else if (model instanceof CompoundModuleNodeEx) {
        	CompoundModuleNodeEx node = (CompoundModuleNodeEx)model;           
			label = ((node.getIsNetwork()) ? "network " : "module ")+node.getName();
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_COMPOUNDMODULE);
		}
		else if (model instanceof SubmoduleNodeEx) {
        	SubmoduleNodeEx node = (SubmoduleNodeEx)model;
			label = node.getName()+" : "+node.getType();
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_SUBMODULE);
		}
		else if (model instanceof ModuleInterfaceNode) {
			ModuleInterfaceNode node = (ModuleInterfaceNode)model;
			label = "interface " + node.getName();
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_COMPOUNDMODULE);
		}
		else if (model instanceof ChannelInterfaceNode) {
			ChannelInterfaceNode node = (ChannelInterfaceNode)model;
			label = "channelinterface "+node.getName();
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNEL);
		} 
		else if (model instanceof ChannelSpecNode) {
			ChannelSpecNode node = (ChannelSpecNode)model;
			label = "channel "+node.getType();
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNEL);
		} 
		else if (model instanceof ChannelNode) {
			ChannelNode node = (ChannelNode)model;
			label = "channel "+node.getName();
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNEL);
		} 
		else if (model instanceof GateNode) {
			GateNode node = (GateNode)model;
        	String attType = node.getAttribute(ParamNode.ATT_TYPE);
			label = "".equals(attType) ? "" : attType+" ";
			label += node.getName();
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_GATE);
		} 
		else if (model instanceof ConnectionNodeEx) {
			ConnectionNodeEx node = (ConnectionNodeEx)model;
			String srclabel = node.getSrcModule();
			srclabel += !"".equals(node.getSrcModuleIndex()) ? 
						     "["+node.getSrcModuleIndex()+"]" : "";
			srclabel += "."+node.getSrcGate();
			srclabel += !"".equals(node.getSrcGateIndex()) ? 
					         "["+node.getSrcGateIndex()+"]" : "";
			String destlabel = node.getDestModule();
			destlabel += !"".equals(node.getDestModuleIndex()) ? 
						"["+node.getDestModuleIndex()+"]" : "";
			destlabel += "."+node.getDestGate();
			destlabel += !"".equals(node.getDestGateIndex()) ? 
				       	"["+node.getDestGateIndex()+"]" : "";
            switch (node.getArrowDirection()) {
                case NedElementExUtil.NED_ARROWDIR_L2R :
                    label = srclabel + " --> " + destlabel;
                    break;
                case NedElementExUtil.NED_ARROWDIR_R2L :
                    label = destlabel + " <-- " + srclabel;
                    break;
                default :
                    label = srclabel + " <--> " + destlabel;
                break;
            }
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CONNECTION);
		} 
		else {
			label = model.getTagName();
            image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_INVALID);
		}
        
    	TreeItem item = (TreeItem)getWidget();
    	if (image != null)
    		image.setBackground(item.getParent().getBackground());
    	setWidgetImage(image);
    	setWidgetText(label);
    }

    
	public void attributeChanged(NEDElement node, String attr) {
        refreshVisuals();
	}

	public void childInserted(NEDElement node, NEDElement where, NEDElement child) {
        refreshVisuals();
	}

	public void childRemoved(NEDElement node, NEDElement child) {
        refreshVisuals();
	}
}