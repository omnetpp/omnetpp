/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned2.provider.tree;

import java.util.Collection;
import java.util.List;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.common.util.ResourceLocator;

import org.eclipse.emf.ecore.EStructuralFeature;

import org.eclipse.emf.edit.provider.ComposeableAdapterFactory;
import org.eclipse.emf.edit.provider.IEditingDomainItemProvider;
import org.eclipse.emf.edit.provider.IItemLabelProvider;
import org.eclipse.emf.edit.provider.IItemPropertySource;
import org.eclipse.emf.edit.provider.IStructuredItemContentProvider;
import org.eclipse.emf.edit.provider.ITreeItemContentProvider;
import org.eclipse.emf.edit.provider.ItemPropertyDescriptor;
import org.eclipse.emf.edit.provider.ItemProviderAdapter;
import org.eclipse.emf.edit.provider.ViewerNotification;

import org.omnetpp.ned2.model.Connection;

import org.omnetpp.ned2.model.meta.Ned2Factory;
import org.omnetpp.ned2.model.meta.Ned2Package;

/**
 * This is the item provider adapter for a {@link org.omnetpp.ned2.model.Connection} object.
 * <!-- begin-user-doc -->
 * <!-- end-user-doc -->
 * @generated
 */
public class ConnectionItemProvider extends ItemProviderAdapter implements
		IEditingDomainItemProvider, IStructuredItemContentProvider,
		ITreeItemContentProvider, IItemLabelProvider, IItemPropertySource {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "";

	/**
	 * This constructs an instance from a factory and a notifier.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ConnectionItemProvider(AdapterFactory adapterFactory) {
		super(adapterFactory);
	}

	/**
	 * This returns the property descriptors for the adapted class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public List getPropertyDescriptors(Object object) {
		if (itemPropertyDescriptors == null) {
			super.getPropertyDescriptors(object);

			addArrowDirectionPropertyDescriptor(object);
			addChannelNamePropertyDescriptor(object);
			addDestGatePropertyDescriptor(object);
			addDestGateIndexPropertyDescriptor(object);
			addDestGatePlusplusPropertyDescriptor(object);
			addDestGateSubgPropertyDescriptor(object);
			addDestModulePropertyDescriptor(object);
			addDestModuleIndexPropertyDescriptor(object);
			addSrcGatePropertyDescriptor(object);
			addSrcGateIndexPropertyDescriptor(object);
			addSrcGatePlusplusPropertyDescriptor(object);
			addSrcGateSubgPropertyDescriptor(object);
			addSrcModulePropertyDescriptor(object);
			addSrcModuleIndexPropertyDescriptor(object);
		}
		return itemPropertyDescriptors;
	}

	/**
	 * This adds a property descriptor for the Arrow Direction feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addArrowDirectionPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_arrowDirection_feature"), getString(
						"_UI_PropertyDescriptor_description",
						"_UI_Connection_arrowDirection_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__ARROW_DIRECTION, true,
				ItemPropertyDescriptor.GENERIC_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Channel Name feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addChannelNamePropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_channelName_feature"), getString(
						"_UI_PropertyDescriptor_description",
						"_UI_Connection_channelName_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__CHANNEL_NAME, true,
				ItemPropertyDescriptor.GENERIC_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Dest Gate feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addDestGatePropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_destGate_feature"), getString(
						"_UI_PropertyDescriptor_description",
						"_UI_Connection_destGate_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__DEST_GATE, true,
				ItemPropertyDescriptor.GENERIC_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Dest Gate Index feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addDestGateIndexPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_destGateIndex_feature"), getString(
						"_UI_PropertyDescriptor_description",
						"_UI_Connection_destGateIndex_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__DEST_GATE_INDEX, true,
				ItemPropertyDescriptor.GENERIC_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Dest Gate Plusplus feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addDestGatePlusplusPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_destGatePlusplus_feature"),
				getString("_UI_PropertyDescriptor_description",
						"_UI_Connection_destGatePlusplus_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__DEST_GATE_PLUSPLUS, true,
				ItemPropertyDescriptor.BOOLEAN_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Dest Gate Subg feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addDestGateSubgPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_destGateSubg_feature"), getString(
						"_UI_PropertyDescriptor_description",
						"_UI_Connection_destGateSubg_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__DEST_GATE_SUBG, true,
				ItemPropertyDescriptor.GENERIC_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Dest Module feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addDestModulePropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_destModule_feature"), getString(
						"_UI_PropertyDescriptor_description",
						"_UI_Connection_destModule_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__DEST_MODULE, true,
				ItemPropertyDescriptor.GENERIC_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Dest Module Index feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addDestModuleIndexPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_destModuleIndex_feature"), getString(
						"_UI_PropertyDescriptor_description",
						"_UI_Connection_destModuleIndex_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__DEST_MODULE_INDEX, true,
				ItemPropertyDescriptor.GENERIC_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Src Gate feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addSrcGatePropertyDescriptor(Object object) {
		itemPropertyDescriptors
				.add(createItemPropertyDescriptor(
						((ComposeableAdapterFactory) adapterFactory)
								.getRootAdapterFactory(), getResourceLocator(),
						getString("_UI_Connection_srcGate_feature"), getString(
								"_UI_PropertyDescriptor_description",
								"_UI_Connection_srcGate_feature",
								"_UI_Connection_type"),
						Ned2Package.Literals.CONNECTION__SRC_GATE, true,
						ItemPropertyDescriptor.GENERIC_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Src Gate Index feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addSrcGateIndexPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_srcGateIndex_feature"), getString(
						"_UI_PropertyDescriptor_description",
						"_UI_Connection_srcGateIndex_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__SRC_GATE_INDEX, true,
				ItemPropertyDescriptor.GENERIC_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Src Gate Plusplus feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addSrcGatePlusplusPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_srcGatePlusplus_feature"), getString(
						"_UI_PropertyDescriptor_description",
						"_UI_Connection_srcGatePlusplus_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__SRC_GATE_PLUSPLUS, true,
				ItemPropertyDescriptor.BOOLEAN_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Src Gate Subg feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addSrcGateSubgPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_srcGateSubg_feature"), getString(
						"_UI_PropertyDescriptor_description",
						"_UI_Connection_srcGateSubg_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__SRC_GATE_SUBG, true,
				ItemPropertyDescriptor.GENERIC_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Src Module feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addSrcModulePropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_srcModule_feature"), getString(
						"_UI_PropertyDescriptor_description",
						"_UI_Connection_srcModule_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__SRC_MODULE, true,
				ItemPropertyDescriptor.GENERIC_VALUE_IMAGE, null, null));
	}

	/**
	 * This adds a property descriptor for the Src Module Index feature.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void addSrcModuleIndexPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add(createItemPropertyDescriptor(
				((ComposeableAdapterFactory) adapterFactory)
						.getRootAdapterFactory(), getResourceLocator(),
				getString("_UI_Connection_srcModuleIndex_feature"), getString(
						"_UI_PropertyDescriptor_description",
						"_UI_Connection_srcModuleIndex_feature",
						"_UI_Connection_type"),
				Ned2Package.Literals.CONNECTION__SRC_MODULE_INDEX, true,
				ItemPropertyDescriptor.GENERIC_VALUE_IMAGE, null, null));
	}

	/**
	 * This specifies how to implement {@link #getChildren} and is used to deduce an appropriate feature for an
	 * {@link org.eclipse.emf.edit.command.AddCommand}, {@link org.eclipse.emf.edit.command.RemoveCommand} or
	 * {@link org.eclipse.emf.edit.command.MoveCommand} in {@link #createCommand}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Collection getChildrenFeatures(Object object) {
		if (childrenFeatures == null) {
			super.getChildrenFeatures(object);
			childrenFeatures.add(Ned2Package.Literals.CONNECTION__WHITESPACE);
			childrenFeatures.add(Ned2Package.Literals.CONNECTION__EXPRESSION);
			childrenFeatures.add(Ned2Package.Literals.CONNECTION__PARAMETERS);
		}
		return childrenFeatures;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EStructuralFeature getChildFeature(Object object, Object child) {
		// Check the type of the specified child object and return the proper feature to use for
		// adding (see {@link AddCommand}) it as a child.

		return super.getChildFeature(object, child);
	}

	/**
	 * This returns Connection.gif.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Object getImage(Object object) {
		return getResourceLocator().getImage("full/obj16/Connection");
	}

	/**
	 * This returns the label text for the adapted class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getText(Object object) {
		String label = ((Connection) object).getChannelName();
		return label == null || label.length() == 0 ? getString("_UI_Connection_type")
				: getString("_UI_Connection_type") + " " + label;
	}

	/**
	 * This handles model notifications by calling {@link #updateChildren} to update any cached
	 * children and by creating a viewer notification, which it passes to {@link #fireNotifyChanged}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void notifyChanged(Notification notification) {
		updateChildren(notification);

		switch (notification.getFeatureID(Connection.class)) {
		case Ned2Package.CONNECTION__ARROW_DIRECTION:
		case Ned2Package.CONNECTION__CHANNEL_NAME:
		case Ned2Package.CONNECTION__DEST_GATE:
		case Ned2Package.CONNECTION__DEST_GATE_INDEX:
		case Ned2Package.CONNECTION__DEST_GATE_PLUSPLUS:
		case Ned2Package.CONNECTION__DEST_GATE_SUBG:
		case Ned2Package.CONNECTION__DEST_MODULE:
		case Ned2Package.CONNECTION__DEST_MODULE_INDEX:
		case Ned2Package.CONNECTION__SRC_GATE:
		case Ned2Package.CONNECTION__SRC_GATE_INDEX:
		case Ned2Package.CONNECTION__SRC_GATE_PLUSPLUS:
		case Ned2Package.CONNECTION__SRC_GATE_SUBG:
		case Ned2Package.CONNECTION__SRC_MODULE:
		case Ned2Package.CONNECTION__SRC_MODULE_INDEX:
			fireNotifyChanged(new ViewerNotification(notification, notification
					.getNotifier(), false, true));
			return;
		case Ned2Package.CONNECTION__WHITESPACE:
		case Ned2Package.CONNECTION__EXPRESSION:
		case Ned2Package.CONNECTION__PARAMETERS:
			fireNotifyChanged(new ViewerNotification(notification, notification
					.getNotifier(), true, false));
			return;
		}
		super.notifyChanged(notification);
	}

	/**
	 * This adds to the collection of {@link org.eclipse.emf.edit.command.CommandParameter}s
	 * describing all of the children that can be created under this object.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void collectNewChildDescriptors(Collection newChildDescriptors,
			Object object) {
		super.collectNewChildDescriptors(newChildDescriptors, object);

		newChildDescriptors.add(createChildParameter(
				Ned2Package.Literals.CONNECTION__WHITESPACE,
				Ned2Factory.eINSTANCE.createWhitespace()));

		newChildDescriptors.add(createChildParameter(
				Ned2Package.Literals.CONNECTION__EXPRESSION,
				Ned2Factory.eINSTANCE.createExpression()));

		newChildDescriptors.add(createChildParameter(
				Ned2Package.Literals.CONNECTION__PARAMETERS,
				Ned2Factory.eINSTANCE.createParameters()));
	}

	/**
	 * Return the resource locator for this item provider's resources.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ResourceLocator getResourceLocator() {
		return Ned2TreeProviderPlugin.INSTANCE;
	}

}
