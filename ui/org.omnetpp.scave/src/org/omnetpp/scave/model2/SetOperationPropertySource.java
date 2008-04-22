package org.omnetpp.scave.model2;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.edit.provider.IItemPropertySource;
import org.eclipse.emf.edit.ui.provider.PropertySource;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.scave.editors.datatable.FilterField;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.SetOperation;
import org.omnetpp.scave.model.provider.ScaveEditPlugin;

/**
 * Property source for SetOperations (Add,Discard,Select and Deselect).
 * The purpose of this class is to provide a content assist for the "Filter Pattern"
 * property.
 * ItemProviders does not give a chance to override the cell editor for a specific
 * feature, so this property source is created for SetOperations. The class
 * delegates all methods to the item provider, but replaces the descriptor of 
 * the "Filter Pattern" property with ours.
 *
 * @author tomi
 */
public class SetOperationPropertySource extends PropertySource {
	
	ResultFileManager manager;

	public SetOperationPropertySource(Object object,
			IItemPropertySource itemPropertySource, ResultFileManager manager) {
		super(object, itemPropertySource);
		Assert.isLegal(object instanceof SetOperation);
		Assert.isLegal(itemPropertySource != null);
		Assert.isLegal(manager != null);
		this.manager = manager;
	}

	public IPropertyDescriptor[] getPropertyDescriptors() {
		IPropertyDescriptor[] descriptors = super.getPropertyDescriptors();

		// find and replace "Filter Pattern" property descriptor
		String filterPatternPropertyId = ScaveEditPlugin.INSTANCE.getString("_UI_SetOperation_filterPattern_feature");
		for (int i = 0; i < descriptors.length; ++i) {
			IPropertyDescriptor descriptor = descriptors[i];
			if (descriptor.getId().equals(filterPatternPropertyId))
				descriptors[i] = new FilterPatternPropertyDescriptor(descriptor);
		}
				
		return descriptors;
	}
	
	class FilterPatternPropertyDescriptor extends PropertyDescriptor
	{
		public FilterPatternPropertyDescriptor(IPropertyDescriptor descriptor) {
			super(descriptor.getId(), descriptor.getDisplayName());
			setDescription(descriptor.getDisplayName());
			setCategory(descriptor.getCategory());
			setFilterFlags(descriptor.getFilterFlags());
		}

		public CellEditor createPropertyEditor(Composite parent) {
			return new FilterPatternCellEditor(parent);
		}
	}
	
	class FilterPatternCellEditor extends TextCellEditor
	{
		public FilterPatternCellEditor(Composite parent) {
			super(parent);
		}

		@Override
		protected Control createControl(Composite parent) {
			Control control =  super.createControl(parent);
			FilterField field = new FilterField(text);
			ResultType resultType = ((SetOperation)object).getType();
			field.setFilterHints(new FilterHints(manager, resultType));
			return control;
		}

		@Override
		protected void doSetValue(Object value) {
			if (value == null)
				value = "";
			super.doSetValue(value);
		}
	}
}
