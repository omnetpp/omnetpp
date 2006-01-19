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

import org.omnetpp.ned.model.emf.CompoundModule;
import org.omnetpp.ned.model.emf.NedFactory;
import org.omnetpp.ned.model.emf.NedPackage;

import org.omnetpp.ned.provider.NedProviderPlugin;

/**
 * This is the item provider adapter for a {@link org.omnetpp.ned.model.emf.CompoundModule} object.
 * <!-- begin-user-doc -->
 * <!-- end-user-doc -->
 * @generated
 */
public class CompoundModuleItemProvider
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
    public CompoundModuleItemProvider(AdapterFactory adapterFactory) {
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
            addNamePropertyDescriptor(object);
            addRightCommentPropertyDescriptor(object);
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
                 getString("_UI_CompoundModule_bannerComment_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_CompoundModule_bannerComment_feature", "_UI_CompoundModule_type"),
                 NedPackage.eINSTANCE.getCompoundModule_BannerComment(),
                 true,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Name feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addNamePropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_CompoundModule_name_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_CompoundModule_name_feature", "_UI_CompoundModule_type"),
                 NedPackage.eINSTANCE.getCompoundModule_Name(),
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
                 getString("_UI_CompoundModule_rightComment_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_CompoundModule_rightComment_feature", "_UI_CompoundModule_type"),
                 NedPackage.eINSTANCE.getCompoundModule_RightComment(),
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
                 getString("_UI_CompoundModule_trailingComment_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_CompoundModule_trailingComment_feature", "_UI_CompoundModule_type"),
                 NedPackage.eINSTANCE.getCompoundModule_TrailingComment(),
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
            childrenFeatures.add(NedPackage.eINSTANCE.getCompoundModule_Params());
            childrenFeatures.add(NedPackage.eINSTANCE.getCompoundModule_Gates());
            childrenFeatures.add(NedPackage.eINSTANCE.getCompoundModule_Submodules());
            childrenFeatures.add(NedPackage.eINSTANCE.getCompoundModule_Connections());
            childrenFeatures.add(NedPackage.eINSTANCE.getCompoundModule_DisplayString());
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
     * This returns CompoundModule.gif.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Object getImage(Object object) {
        return getResourceLocator().getImage("full/obj16/CompoundModule");
    }

    /**
     * This returns the label text for the adapted class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getText(Object object) {
        String label = ((CompoundModule)object).getName();
        return label == null || label.length() == 0 ?
            getString("_UI_CompoundModule_type") :
            getString("_UI_CompoundModule_type") + " " + label;
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

        switch (notification.getFeatureID(CompoundModule.class)) {
            case NedPackage.COMPOUND_MODULE__BANNER_COMMENT:
            case NedPackage.COMPOUND_MODULE__NAME:
            case NedPackage.COMPOUND_MODULE__RIGHT_COMMENT:
            case NedPackage.COMPOUND_MODULE__TRAILING_COMMENT:
                fireNotifyChanged(new ViewerNotification(notification, notification.getNotifier(), false, true));
                return;
            case NedPackage.COMPOUND_MODULE__PARAMS:
            case NedPackage.COMPOUND_MODULE__GATES:
            case NedPackage.COMPOUND_MODULE__SUBMODULES:
            case NedPackage.COMPOUND_MODULE__CONNECTIONS:
            case NedPackage.COMPOUND_MODULE__DISPLAY_STRING:
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
                (NedPackage.eINSTANCE.getCompoundModule_Params(),
                 NedFactory.eINSTANCE.createParams()));

        newChildDescriptors.add
            (createChildParameter
                (NedPackage.eINSTANCE.getCompoundModule_Gates(),
                 NedFactory.eINSTANCE.createGates()));

        newChildDescriptors.add
            (createChildParameter
                (NedPackage.eINSTANCE.getCompoundModule_Submodules(),
                 NedFactory.eINSTANCE.createSubmodules()));

        newChildDescriptors.add
            (createChildParameter
                (NedPackage.eINSTANCE.getCompoundModule_Connections(),
                 NedFactory.eINSTANCE.createConnections()));

        newChildDescriptors.add
            (createChildParameter
                (NedPackage.eINSTANCE.getCompoundModule_DisplayString(),
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
