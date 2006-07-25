package org.omnetpp.scave2.editors.ui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EReference;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.edit.provider.ItemPropertyDescriptor;
import org.eclipse.emf.edit.ui.provider.PropertyDescriptor;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusAdapter;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.model.provider.ScaveModelItemProviderAdapterFactory;

/**
 * 
 *
 * @author tomi
 */
public class GenericScaveObjectEditForm implements IScaveObjectEditForm {

	private EObject object;
	private AdapterFactory adapterFactory;
	private EStructuralFeature[] features;
	private CellEditor[] editors;

	public GenericScaveObjectEditForm(EObject object) {
		this(object, getEditableFeatures(object));
	}
	
	public GenericScaveObjectEditForm(EObject object, EStructuralFeature[] features) {
		this.object = object;
		this.features = features;
		this.adapterFactory = new ScaveModelItemProviderAdapterFactory();
	}
	
	
	public void populatePanel(Composite panel) {
		panel.setLayout(new GridLayout(2, false));
		
		editors = new CellEditor[features.length];
		for (int i = 0; i < features.length; ++i) {
			EStructuralFeature feature = features[i];
			// create label
			Label label = new Label(panel, SWT.NONE);
			label.setText(StringUtils.toDisplayString(feature.getName()) + ":");
			GridData gridData = new GridData();
			gridData.horizontalIndent = 10;
			label.setLayoutData(gridData);
			// create editor
			CellEditor editor = editors[i] = new PropertyDescriptor(object,
				new ItemPropertyDescriptor(
						adapterFactory,
						"",
						"",
						feature,
						feature.isChangeable(),
						null)).createPropertyEditor(panel);
			editor.activate();
			final Control control = editor.getControl();
			gridData = new GridData(SWT.FILL, SWT.TOP, true, false);
			control.setLayoutData(gridData);
			control.setBackground(ColorFactory.asColor("white"));
			// control would get invisible when it loses the focus 
			control.addFocusListener(new FocusAdapter() {
				public void focusLost(FocusEvent e) {
					control.setVisible(true);
				}
			});
			control.setVisible(true);
		}
	}
	
	
	private static EStructuralFeature[] getEditableFeatures(EObject object) {
		List<EStructuralFeature> features = new ArrayList<EStructuralFeature>();
		for (Object feature : object.eClass().getEAllStructuralFeatures())
		 if (!(feature instanceof EReference) || !((EReference)feature).isContainment())
			 features.add((EStructuralFeature)feature);
		return features.toArray(new EStructuralFeature[features.size()]);
	}
	
	
	public EStructuralFeature[] getFeatures() {
		return features;
	}
	
	public int getFeatureCount() {
		return features.length;
	}
	
	public boolean isDirty(int index) {
		return index >= 0 && index < editors.length ? editors[index].isDirty() : false;
	}
	
	public Object getValue(int index) {
		return index >= 0 && index <= editors.length ? editors[index].getValue() : null;
	}

	public void setValue(int index, Object value) {
		if (index >= 0 && index < editors.length)
			editors[index].setValue(value);
	}
}
