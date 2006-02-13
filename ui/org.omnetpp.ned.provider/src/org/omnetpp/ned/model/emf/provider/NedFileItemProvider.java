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

import org.eclipse.emf.ecore.util.FeatureMapUtil;

import org.eclipse.emf.edit.provider.ComposeableAdapterFactory;
import org.eclipse.emf.edit.provider.IEditingDomainItemProvider;
import org.eclipse.emf.edit.provider.IItemLabelProvider;
import org.eclipse.emf.edit.provider.IItemPropertySource;
import org.eclipse.emf.edit.provider.IStructuredItemContentProvider;
import org.eclipse.emf.edit.provider.ITreeItemContentProvider;
import org.eclipse.emf.edit.provider.ItemPropertyDescriptor;
import org.eclipse.emf.edit.provider.ItemProviderAdapter;
import org.eclipse.emf.edit.provider.ViewerNotification;

import org.omnetpp.ned.model.emf.NedFactory;
import org.omnetpp.ned.model.emf.NedFile;
import org.omnetpp.ned.model.emf.NedPackage;

import org.omnetpp.ned.provider.NedProviderPlugin;

/**
 * This is the item provider adapter for a {@link org.omnetpp.ned.model.emf.NedFile} object.
 * <!-- begin-user-doc -->
 * <!-- end-user-doc -->
 * @generated
 */
public class NedFileItemProvider
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
    public NedFileItemProvider(AdapterFactory adapterFactory) {
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
                 getString("_UI_NedFile_bannerComment_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_NedFile_bannerComment_feature", "_UI_NedFile_type"),
                 NedPackage.eINSTANCE.getNedFile_BannerComment(),
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
                 getString("_UI_NedFile_rightComment_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_NedFile_rightComment_feature", "_UI_NedFile_type"),
                 NedPackage.eINSTANCE.getNedFile_RightComment(),
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
                 getString("_UI_NedFile_trailingComment_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_NedFile_trailingComment_feature", "_UI_NedFile_type"),
                 NedPackage.eINSTANCE.getNedFile_TrailingComment(),
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
            childrenFeatures.add(NedPackage.eINSTANCE.getNedFile_Group());
        }
        return childrenFeatures;
    }

    /**
     * This returns NedFile.gif.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Object getImage(Object object) {
        return getResourceLocator().getImage("full/obj16/NedFile");
    }

    /**
     * This returns the label text for the adapted class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getText(Object object) {
        String label = ((NedFile)object).getBannerComment();
        return label == null || label.length() == 0 ?
            getString("_UI_NedFile_type") :
            getString("_UI_NedFile_type") + " " + label;
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

        switch (notification.getFeatureID(NedFile.class)) {
            case NedPackage.NED_FILE__BANNER_COMMENT:
            case NedPackage.NED_FILE__RIGHT_COMMENT:
            case NedPackage.NED_FILE__TRAILING_COMMENT:
                fireNotifyChanged(new ViewerNotification(notification, notification.getNotifier(), false, true));
                return;
            case NedPackage.NED_FILE__GROUP:
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
                (NedPackage.eINSTANCE.getNedFile_Group(),
                 FeatureMapUtil.createEntry
                    (NedPackage.eINSTANCE.getNedFile_Import(),
                     NedFactory.eINSTANCE.createImport())));

        newChildDescriptors.add
            (createChildParameter
                (NedPackage.eINSTANCE.getNedFile_Group(),
                 FeatureMapUtil.createEntry
                    (NedPackage.eINSTANCE.getNedFile_Channel(),
                     NedFactory.eINSTANCE.createChannel())));

        newChildDescriptors.add
            (createChildParameter
                (NedPackage.eINSTANCE.getNedFile_Group(),
                 FeatureMapUtil.createEntry
                    (NedPackage.eINSTANCE.getNedFile_SimpleModule(),
                     NedFactory.eINSTANCE.createSimpleModule())));

        newChildDescriptors.add
            (createChildParameter
                (NedPackage.eINSTANCE.getNedFile_Group(),
                 FeatureMapUtil.createEntry
                    (NedPackage.eINSTANCE.getNedFile_CompoundModule(),
                     NedFactory.eINSTANCE.createCompoundModule())));

        newChildDescriptors.add
            (createChildParameter
                (NedPackage.eINSTANCE.getNedFile_Group(),
                 FeatureMapUtil.createEntry
                    (NedPackage.eINSTANCE.getNedFile_Network(),
                     NedFactory.eINSTANCE.createNetwork())));
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
