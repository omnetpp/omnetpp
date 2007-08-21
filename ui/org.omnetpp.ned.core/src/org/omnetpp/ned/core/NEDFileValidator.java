package org.omnetpp.ned.core;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDElementConstants;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.ChannelInterfaceElement;
import org.omnetpp.ned.model.pojo.ChannelElement;
import org.omnetpp.ned.model.pojo.CompoundModuleElement;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.NedFileElement;
import org.omnetpp.ned.model.pojo.PropertyDeclElement;
import org.omnetpp.ned.model.pojo.PropertyElement;
import org.omnetpp.ned.model.pojo.SimpleModuleElement;

/**
 * Validates consistency of NED files.
 *
 * @author andras
 */
//FIXME validation of embedded types!!!!
//FIXME check that a module/channel satisfies the interfaces it implements!
//FIXME finish validator functions! e.g. turn on expression parsing
public class NEDFileValidator implements NEDElementConstants, NEDElementTags {
	INEDTypeResolver resolver;
	INEDErrorStore errors;

	public NEDFileValidator(INEDTypeResolver resolver, INEDErrorStore errors) {
		this.resolver = resolver;
		this.errors = errors;
	}

	public void validate(NedFileElement node) {
		for (INEDElement child : node) {
			NEDValidator validator = new NEDValidator(resolver, errors);
			switch (child.getTagCode()) {
			case NED_COMMENT: break;
			case NED_IMPORT: break;
			case NED_PROPERTY_DECL: validator.validate((PropertyDeclElement) child); break;
			case NED_SIMPLE_MODULE: validator.validate((SimpleModuleElement) child); break;
			case NED_MODULE_INTERFACE: validator.validate((ModuleInterfaceElement) child); break;
			case NED_COMPOUND_MODULE: validator.validate((CompoundModuleElement) child); break;
			case NED_CHANNEL_INTERFACE: validator.validate((ChannelInterfaceElement) child); break;
			case NED_CHANNEL: validator.validate((ChannelElement) child); break;
			case NED_PROPERTY: validator.validate((PropertyElement) child); break;
			default: Assert.isTrue(false, "unexpected element type: "+child.getTagName());
			}
		}
	}
}
