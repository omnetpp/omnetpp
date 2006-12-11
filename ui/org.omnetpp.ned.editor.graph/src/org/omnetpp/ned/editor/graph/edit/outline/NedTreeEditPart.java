package org.omnetpp.ned.editor.graph.edit.outline;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.RootEditPart;
import org.eclipse.gef.editparts.AbstractTreeEditPart;
import org.eclipse.gef.editpolicies.RootComponentEditPolicy;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedTreeContainerEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedTreeEditPolicy;
import org.omnetpp.ned.editor.graph.properties.IPropertySourceSupport;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDElementUtil;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.ConnectionNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned.model.pojo.ChannelNode;
import org.omnetpp.ned.model.pojo.ChannelSpecNode;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.ConnectionGroupNode;
import org.omnetpp.ned.model.pojo.ConnectionNode;
import org.omnetpp.ned.model.pojo.ConnectionsNode;
import org.omnetpp.ned.model.pojo.GateNode;
import org.omnetpp.ned.model.pojo.GatesNode;
import org.omnetpp.ned.model.pojo.ImportNode;
import org.omnetpp.ned.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned.model.pojo.NedFileNode;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.ParametersNode;
import org.omnetpp.ned.model.pojo.PropertyNode;
import org.omnetpp.ned.model.pojo.SimpleModuleNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;
import org.omnetpp.ned.model.pojo.SubmodulesNode;
import org.omnetpp.ned.model.pojo.TypesNode;

/**
 * EditPart for Logic components in the Tree.
 */
public class NedTreeEditPart extends AbstractTreeEditPart implements
        INEDChangeListener, IPropertySourceSupport {

    private long lastEventSerial;
    private IPropertySource propertySource;

    /**
     * Constructor initializes this with the given model.
     * 
     * @param model
     *            Model for this.
     */
    public NedTreeEditPart(Object model) {
        super(model);
    }

    @Override
    public void activate() {
        super.activate();
        ((NEDElement)getModel()).getListeners().add(this);
    }

    /**
     * Creates and installs pertinent EditPolicies for this.
     */
    @Override
    protected void createEditPolicies() {
        // install root policy to disable deleting of the root node
        if (getParent() instanceof RootEditPart)
            installEditPolicy(EditPolicy.COMPONENT_ROLE, new RootComponentEditPolicy());

    	installEditPolicy(EditPolicy.PRIMARY_DRAG_ROLE, new NedTreeEditPolicy());
    	installEditPolicy(EditPolicy.TREE_CONTAINER_ROLE, new NedTreeContainerEditPolicy());

        // we do not allow the reordering of the content (children) of the following node types 
    	if (getModel() instanceof SubmoduleNodeEx || 
        	getModel() instanceof CompoundModuleNodeEx ) {
        	removeEditPolicy(EditPolicy.TREE_CONTAINER_ROLE);
        }
    	
        // delete support
        installEditPolicy(EditPolicy.COMPONENT_ROLE, new NedComponentEditPolicy());
        // reorder support
    }

    @Override
    public void deactivate() {
        ((NEDElement)getModel()).getListeners().remove(this);
        super.deactivate();
    }

    /**
     * Returns the children of this from the model, as this is capable enough of
     * holding EditParts.
     * 
     * @return List of children.
     */
    @Override
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
        			(child instanceof ParamNode) ||
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
			StringBuffer srclabel = new StringBuffer();
            if (!"".equals(node.getSrcModule())) {
                srclabel.append(node.getSrcModule());
                srclabel.append(!"".equals(node.getSrcModuleIndex()) ? 
                                 "["+node.getSrcModuleIndex()+"]" : "");
                srclabel.append(".");
            }
			srclabel.append(node.getSrcGate());
			srclabel.append(!"".equals(node.getSrcGateIndex()) ? 
					         "["+node.getSrcGateIndex()+"]" : "");
			
            StringBuffer destlabel = new StringBuffer();
            if(!"".equals(node.getDestModule())) {
                destlabel.append(node.getDestModule());
                destlabel.append(!"".equals(node.getDestModuleIndex()) ? 
                        "["+node.getDestModuleIndex()+"]" : "");
                destlabel.append(".");
            }
			destlabel.append(node.getDestGate());
			destlabel.append(!"".equals(node.getDestGateIndex()) ? 
				       	"["+node.getDestGateIndex()+"]" : "");
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

    public void modelChanged(NEDModelEvent event) {
        // skip the event processing if te last serial is greater or equal. only newer
        // events should be processed. this prevent the processing of the same event multiple times
        if (lastEventSerial >= event.getSerial())
            return;
        else // process the even and remeber this serial
            lastEventSerial = event.getSerial();

        refreshChildren();
    }

    public IPropertySource getPropertySource() {
        return propertySource;
    }

    public void setPropertySource(IPropertySource propertySource) {
        this.propertySource = propertySource;
    }
}