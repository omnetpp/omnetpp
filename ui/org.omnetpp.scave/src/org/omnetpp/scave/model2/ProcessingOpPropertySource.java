package org.omnetpp.scave.model2;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.common.properties.BasePropertySource;
import org.omnetpp.common.properties.EditableComboBoxPropertyDescriptor;
import org.omnetpp.common.properties.Property;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.scave.engine.NodeType;
import org.omnetpp.scave.engine.NodeTypeRegistry;
import org.omnetpp.scave.engine.NodeTypeVector;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.model.Param;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * PropertySource implementation for Apply/Compute.
 *
 * @author tomi
 */
public class ProcessingOpPropertySource extends PropertySource {

	private ProcessingOp processingOp;
	private EditingDomain domain;

	private static final ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
	private static final NodeTypeRegistry registry = NodeTypeRegistry.getInstance();
	private static final String[] nodeTypes;
	static {
		NodeTypeVector nodeTypeVector = registry.getNodeTypes();
		List<String> filterNodeTypes = new ArrayList<String>();
		for (int i = 0; i < nodeTypeVector.size(); ++i) {
			NodeType nodeType = nodeTypeVector.get(i);
			if ("filter".equals(nodeType.getCategory()))
				filterNodeTypes.add(nodeTypeVector.get(i).getName());
		}
		nodeTypes = filterNodeTypes.toArray(new String[filterNodeTypes.size()]);
	}
	
	public ProcessingOpPropertySource(ProcessingOp processingOp) {
		this.processingOp = processingOp;
		this.domain = AdapterFactoryEditingDomain.getEditingDomainFor(processingOp);
	}
	
	@Property
	public String getOperation() {
		return processingOp.getOperation();
	}
	
	public void setOperation(String op) {
		if (domain == null) {
			processingOp.setOperation(op);
			processingOp.getParams().clear();
		}
		else {
			CompoundCommand command = new CompoundCommand("Set operation");
			command.append(SetCommand.create(domain, processingOp, pkg.getProcessingOp_Operation(), op));
			command.append(RemoveCommand.create(domain, processingOp, pkg.getProcessingOp_Params(), processingOp.getParams()));
			domain.getCommandStack().execute(command);
		}
	}
	
	public PropertyDescriptor createOperationDescriptor(Object id, String displayName) {
		return new EditableComboBoxPropertyDescriptor(id, displayName, nodeTypes);
	}
	
	@Property
	public IPropertySource getParameters() {
		String op = processingOp.getOperation();
		if (op != null && registry.exists(op)) {
			NodeType nodeType = registry.getNodeType(op);
			if ("filter".equals(nodeType.getCategory()))
				return new ParamsPropertySource(nodeType, processingOp.getParams());
		}
		return new BasePropertySource();
	}
	
	private class ParamsPropertySource implements IPropertySource2 {
		
		List<Param> params;
		StringMap paramDefs;
		StringMap defaultValues;

		public ParamsPropertySource(NodeType nodeType, List<Param> params) {
			this.params = params;
			this.paramDefs = new StringMap();
			this.defaultValues = new StringMap();
			nodeType.getAttributes(paramDefs);
			nodeType.getAttrDefaults(defaultValues);
		}

		public IPropertyDescriptor[] getPropertyDescriptors() {
			IPropertyDescriptor[] descriptors = new IPropertyDescriptor[(int)paramDefs.size()];
			int i = 0;
			for (String key : paramDefs.keys().toArray()) {
				PropertyDescriptor descriptor = new TextPropertyDescriptor(key, key);
				descriptor.setDescription(paramDefs.get(key));
				descriptors[i++] = descriptor;
			}
			return descriptors;
		}

		public Object getPropertyValue(Object id) {
			Assert.isLegal(id instanceof String);
			for (Param param : params)
				if (id.equals(param.getName()))
					return param.getValue();
			return defaultValues.has_key((String)id) ? defaultValues.get((String)id) : "";
		}

		public void setPropertyValue(Object id, Object value) {
			Assert.isLegal(id instanceof String);
			Assert.isLegal(value == null || value instanceof String);
			for (Param param : params)
				if (id.equals(param.getName())) {
					if (domain == null)
						param.setValue((String)value);
					else
						domain.getCommandStack().execute(
								SetCommand.create(domain, param, pkg.getParam_Value(), value));
					return;
				}
			Param param = ScaveModelFactory.eINSTANCE.createParam();
			param.setName((String)id);
			param.setValue((String)value);
			if (domain == null)
				params.add(param);
			else
				domain.getCommandStack().execute(
						AddCommand.create(domain, processingOp, pkg.getProcessingOp_Params(), param));
		}

		public boolean isPropertyResettable(Object id) {
			Assert.isLegal(id instanceof String);
			return defaultValues.has_key((String)id);
		}

		public void resetPropertyValue(Object id) {
			Assert.isLegal(id instanceof String);
			setPropertyValue(id, defaultValues.get((String)id));
		}
		
		public boolean isPropertySet(Object id) {
			Assert.isLegal(id instanceof String);
			if (defaultValues.has_key((String)id))
				return !defaultValues.get((String)id).equals(getPropertyValue(id));
			else
				return true;
		}

		public Object getEditableValue() {
			return null;
		}
	}
}
