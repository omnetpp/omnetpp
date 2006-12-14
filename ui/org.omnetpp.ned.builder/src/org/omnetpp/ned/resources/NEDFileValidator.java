package org.omnetpp.ned.resources;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDElementUtil;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned.model.pojo.ChannelNode;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.NedFileNode;
import org.omnetpp.ned.model.pojo.PropertyDeclNode;
import org.omnetpp.ned.model.pojo.PropertyNode;
import org.omnetpp.ned.model.pojo.SimpleModuleNode;

/**
 * Validates consistency of NED files.
 * 
 * @author andras
 */
//XXX move to org.omnetpp.ned.model plugin? then INEDComponent,
//INEDTypeResolver etc would have to be moved as well, and that plugin
//would have to depend on org.eclipse.resources because of IFile!!!

//FIXME validation of embedded types!!!!
//FIXME finish validator functions! e.g. turn on expression parsing

public class NEDFileValidator implements NEDElementUtil, NEDElementTags {
	INEDTypeResolver resolver;
	INEDErrorStore errors;

	public NEDFileValidator(INEDTypeResolver resolver, INEDErrorStore errors) {
		this.resolver = resolver;
		this.errors = errors;
	}

	public void validate(NedFileNode node) {
		for (NEDElement child : node) {
			NEDValidator validator = new NEDValidator(resolver, errors);
			switch (child.getTagCode()) {
			case NED_COMMENT: break;
			case NED_IMPORT: break;
			case NED_PROPERTY_DECL: validator.validate((PropertyDeclNode) child); break;
			case NED_SIMPLE_MODULE: validator.validate((SimpleModuleNode) child); break;
			case NED_MODULE_INTERFACE: validator.validate((ModuleInterfaceNode) child); break;
			case NED_COMPOUND_MODULE: validator.validate((CompoundModuleNode) child); break;
			case NED_CHANNEL_INTERFACE: validator.validate((ChannelInterfaceNode) child); break;
			case NED_CHANNEL: validator.validate((ChannelNode) child); break;
			case NED_PROPERTY: validator.validate((PropertyNode) child); break;
			default: Assert.isTrue(false, "unexpected element type: "+child.getTagName());
			}
		}
	}
}
