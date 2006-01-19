/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf.provider;


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

import org.omnetpp.ned.model.emf.Connection;
import org.omnetpp.ned.model.emf.NedFactory;
import org.omnetpp.ned.model.emf.NedPackage;

import org.omnetpp.ned.provider.NedProviderPlugin;

/**
 * This is the item provider adapter for a {@link org.omnetpp.ned.model.emf.Connection} object.
 * <!-- begin-user-doc -->
 * <!-- end-user-doc -->
 * @generated
 */
public class ConnectionItemProvider
    extends ItemProviderAdapter
    implements	
        IEditingDomainItemProvider,	
        IStructuredItemContentProvider,	
        ITreeItemContentProvider,	
        IItemLabelProvider,	
        IItemPropertySource {
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

            addBannerCommentPropertyDescriptor(object);
            addConditionPropertyDescriptor(object);
            addDestGatePropertyDescriptor(object);
            addDestGateIndexPropertyDescriptor(object);
            addDestGatePlusplusPropertyDescriptor(object);
            addDestModulePropertyDescriptor(object);
            addDestModuleIndexPropertyDescriptor(object);
            addRightCommentPropertyDescriptor(object);
            addRightToLeftPropertyDescriptor(object);
            addSrcGatePropertyDescriptor(object);
            addSrcGateIndexPropertyDescriptor(object);
            addSrcGatePlusplusPropertyDescriptor(object);
            addSrcModulePropertyDescriptor(object);
            addSrcModuleIndexPropertyDescriptor(object);
            addTrailingCommentPropertyDescriptor(object);
        }
        return itemPropertyDescriptors;
    }

    /**
     * This adds a property descriptor for the Banner Comment feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addBannerCommentPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_bannerComment_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_bannerComment_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_BannerComment(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Condition feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addConditionPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_condition_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_condition_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_Condition(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Dest Gate feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addDestGatePropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_destGate_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_destGate_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_DestGate(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Dest Gate Index feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addDestGateIndexPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_destGateIndex_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_destGateIndex_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_DestGateIndex(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Dest Gate Plusplus feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addDestGatePlusplusPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_destGatePlusplus_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_destGatePlusplus_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_DestGatePlusplus(),
                 true,
                 ItemPropertyDescriptor.BOOLEAN_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Dest Module feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addDestModulePropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_destModule_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_destModule_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_DestModule(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Dest Module Index feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addDestModuleIndexPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_destModuleIndex_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_destModuleIndex_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_DestModuleIndex(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Right Comment feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addRightCommentPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_rightComment_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_rightComment_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_RightComment(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Right To Left feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addRightToLeftPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_rightToLeft_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_rightToLeft_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_RightToLeft(),
                 true,
                 ItemPropertyDescriptor.BOOLEAN_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Src Gate feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addSrcGatePropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_srcGate_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_srcGate_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_SrcGate(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Src Gate Index feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addSrcGateIndexPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_srcGateIndex_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_srcGateIndex_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_SrcGateIndex(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Src Gate Plusplus feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addSrcGatePlusplusPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_srcGatePlusplus_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_srcGatePlusplus_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_SrcGatePlusplus(),
                 true,
                 ItemPropertyDescriptor.BOOLEAN_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Src Module feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addSrcModulePropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_srcModule_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_srcModule_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_SrcModule(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Src Module Index feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addSrcModuleIndexPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_srcModuleIndex_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_srcModuleIndex_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_SrcModuleIndex(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Trailing Comment feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addTrailingCommentPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_Connection_trailingComment_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_Connection_trailingComment_feature", "_UI_Connection_type"),
                 NedPackage.eINSTANCE.getConnection_TrailingComment(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
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
            childrenFeatures.add(NedPackage.eINSTANCE.getConnection_ConnAttr());
            childrenFeatures.add(NedPackage.eINSTANCE.getConnection_DisplayString());
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
        String label = ((Connection)object).getBannerComment();
        return label == null || label.length() == 0 ?
            getString("_UI_Connection_type") :
            getString("_UI_Connection_type") + " " + label;
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
            case NedPackage.CONNECTION__BANNER_COMMENT:
            case NedPackage.CONNECTION__CONDITION:
            case NedPackage.CONNECTION__DEST_GATE:
            case NedPackage.CONNECTION__DEST_GATE_INDEX:
            case NedPackage.CONNECTION__DEST_GATE_PLUSPLUS:
            case NedPackage.CONNECTION__DEST_MODULE:
            case NedPackage.CONNECTION__DEST_MODULE_INDEX:
            case NedPackage.CONNECTION__RIGHT_COMMENT:
            case NedPackage.CONNECTION__RIGHT_TO_LEFT:
            case NedPackage.CONNECTION__SRC_GATE:
            case NedPackage.CONNECTION__SRC_GATE_INDEX:
            case NedPackage.CONNECTION__SRC_GATE_PLUSPLUS:
            case NedPackage.CONNECTION__SRC_MODULE:
            case NedPackage.CONNECTION__SRC_MODULE_INDEX:
            case NedPackage.CONNECTION__TRAILING_COMMENT:
                fireNotifyChanged(new ViewerNotification(notification, notification.getNotifier(), false, true));
                return;
            case NedPackage.CONNECTION__CONN_ATTR:
            case NedPackage.CONNECTION__DISPLAY_STRING:
                fireNotifyChanged(new ViewerNotification(notification, notification.getNotifier(), true, false));
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
    protected void collectNewChildDescriptors(Collection newChildDescriptors, Object object) {
        super.collectNewChildDescriptors(newChildDescriptors, object);

        newChildDescriptors.add
            (createChildParameter
                (NedPackage.eINSTANCE.getConnection_ConnAttr(),
                 NedFactory.eINSTANCE.createConnAttr()));

        newChildDescriptors.add
            (createChildParameter
                (NedPackage.eINSTANCE.getConnection_DisplayString(),
                 NedFactory.eINSTANCE.createDisplayString()));
    }

    /**
     * Return the resource locator for this item provider's resources.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ResourceLocator getResourceLocator() {
        return NedProviderPlugin.INSTANCE;
    }

}
