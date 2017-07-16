/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model.provider;


import java.util.Collection;
import java.util.List;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EStructuralFeature;

import org.eclipse.emf.edit.provider.ComposeableAdapterFactory;
import org.eclipse.emf.edit.provider.IEditingDomainItemProvider;
import org.eclipse.emf.edit.provider.IItemLabelProvider;
import org.eclipse.emf.edit.provider.IItemPropertyDescriptor;
import org.eclipse.emf.edit.provider.IItemPropertySource;
import org.eclipse.emf.edit.provider.IStructuredItemContentProvider;
import org.eclipse.emf.edit.provider.ITreeItemContentProvider;
import org.eclipse.emf.edit.provider.ItemPropertyDescriptor;
import org.eclipse.emf.edit.provider.ViewerNotification;

import org.omnetpp.scave.model.ComputeScalar;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * This is the item provider adapter for a {@link org.omnetpp.scave.model.ComputeScalar} object.
 * <!-- begin-user-doc -->
 * <!-- end-user-doc -->
 * @generated
 */
public class ComputeScalarItemProvider
    extends DatasetItemItemProvider {
    /**
     * This constructs an instance from a factory and a notifier.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ComputeScalarItemProvider(AdapterFactory adapterFactory) {
        super(adapterFactory);
    }

    /**
     * This returns the property descriptors for the adapted class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public List<IItemPropertyDescriptor> getPropertyDescriptors(Object object) {
        if (itemPropertyDescriptors == null) {
            super.getPropertyDescriptors(object);

            addScalarNamePropertyDescriptor(object);
            addValueExprPropertyDescriptor(object);
            addModuleExprPropertyDescriptor(object);
            addGroupByExprPropertyDescriptor(object);
            addAverageReplicationsPropertyDescriptor(object);
            addComputeStddevPropertyDescriptor(object);
            addComputeConfidenceIntervalPropertyDescriptor(object);
            addConfidenceLevelPropertyDescriptor(object);
            addComputeMinMaxPropertyDescriptor(object);
        }
        return itemPropertyDescriptors;
    }

    /**
     * This adds a property descriptor for the Scalar Name feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addScalarNamePropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_ComputeScalar_scalarName_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_ComputeScalar_scalarName_feature", "_UI_ComputeScalar_type"),
                 ScaveModelPackage.Literals.COMPUTE_SCALAR__SCALAR_NAME,
                 true,
                 false,
                 false,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Value Expr feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addValueExprPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_ComputeScalar_valueExpr_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_ComputeScalar_valueExpr_feature", "_UI_ComputeScalar_type"),
                 ScaveModelPackage.Literals.COMPUTE_SCALAR__VALUE_EXPR,
                 true,
                 false,
                 false,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Module Expr feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addModuleExprPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_ComputeScalar_moduleExpr_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_ComputeScalar_moduleExpr_feature", "_UI_ComputeScalar_type"),
                 ScaveModelPackage.Literals.COMPUTE_SCALAR__MODULE_EXPR,
                 true,
                 false,
                 false,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Group By Expr feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addGroupByExprPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_ComputeScalar_groupByExpr_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_ComputeScalar_groupByExpr_feature", "_UI_ComputeScalar_type"),
                 ScaveModelPackage.Literals.COMPUTE_SCALAR__GROUP_BY_EXPR,
                 true,
                 false,
                 false,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Average Replications feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addAverageReplicationsPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_ComputeScalar_averageReplications_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_ComputeScalar_averageReplications_feature", "_UI_ComputeScalar_type"),
                 ScaveModelPackage.Literals.COMPUTE_SCALAR__AVERAGE_REPLICATIONS,
                 true,
                 false,
                 false,
                 ItemPropertyDescriptor.BOOLEAN_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Compute Stddev feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addComputeStddevPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_ComputeScalar_computeStddev_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_ComputeScalar_computeStddev_feature", "_UI_ComputeScalar_type"),
                 ScaveModelPackage.Literals.COMPUTE_SCALAR__COMPUTE_STDDEV,
                 true,
                 false,
                 false,
                 ItemPropertyDescriptor.BOOLEAN_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Compute Confidence Interval feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addComputeConfidenceIntervalPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_ComputeScalar_computeConfidenceInterval_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_ComputeScalar_computeConfidenceInterval_feature", "_UI_ComputeScalar_type"),
                 ScaveModelPackage.Literals.COMPUTE_SCALAR__COMPUTE_CONFIDENCE_INTERVAL,
                 true,
                 false,
                 false,
                 ItemPropertyDescriptor.BOOLEAN_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Confidence Level feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addConfidenceLevelPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_ComputeScalar_confidenceLevel_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_ComputeScalar_confidenceLevel_feature", "_UI_ComputeScalar_type"),
                 ScaveModelPackage.Literals.COMPUTE_SCALAR__CONFIDENCE_LEVEL,
                 true,
                 false,
                 false,
                 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
                 null,
                 null));
    }

    /**
     * This adds a property descriptor for the Compute Min Max feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void addComputeMinMaxPropertyDescriptor(Object object) {
        itemPropertyDescriptors.add
            (createItemPropertyDescriptor
                (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
                 getResourceLocator(),
                 getString("_UI_ComputeScalar_computeMinMax_feature"),
                 getString("_UI_PropertyDescriptor_description", "_UI_ComputeScalar_computeMinMax_feature", "_UI_ComputeScalar_type"),
                 ScaveModelPackage.Literals.COMPUTE_SCALAR__COMPUTE_MIN_MAX,
                 true,
                 false,
                 false,
                 ItemPropertyDescriptor.BOOLEAN_VALUE_IMAGE,
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
    @Override
    public Collection<? extends EStructuralFeature> getChildrenFeatures(Object object) {
        if (childrenFeatures == null) {
            super.getChildrenFeatures(object);
            childrenFeatures.add(ScaveModelPackage.Literals.COMPUTE_SCALAR__FILTERS);
        }
        return childrenFeatures;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EStructuralFeature getChildFeature(Object object, Object child) {
        // Check the type of the specified child object and return the proper feature to use for
        // adding (see {@link AddCommand}) it as a child.

        return super.getChildFeature(object, child);
    }

    /**
     * This returns ComputeScalar.gif.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public Object getImage(Object object) {
        return overlayImage(object, getResourceLocator().getImage("full/obj16/ComputeScalar"));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected boolean shouldComposeCreationImage() {
        return true;
    }

    /**
     * This returns the label text for the adapted class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public String getText(Object object) {
        String label = ((ComputeScalar)object).getScalarName();
        return label == null || label.length() == 0 ?
            getString("_UI_ComputeScalar_type") :
            getString("_UI_ComputeScalar_type") + " " + label;
    }

    /**
     * This handles model notifications by calling {@link #updateChildren} to update any cached
     * children and by creating a viewer notification, which it passes to {@link #fireNotifyChanged}.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public void notifyChanged(Notification notification) {
        updateChildren(notification);

        switch (notification.getFeatureID(ComputeScalar.class)) {
            case ScaveModelPackage.COMPUTE_SCALAR__SCALAR_NAME:
            case ScaveModelPackage.COMPUTE_SCALAR__VALUE_EXPR:
            case ScaveModelPackage.COMPUTE_SCALAR__MODULE_EXPR:
            case ScaveModelPackage.COMPUTE_SCALAR__GROUP_BY_EXPR:
            case ScaveModelPackage.COMPUTE_SCALAR__AVERAGE_REPLICATIONS:
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_STDDEV:
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_CONFIDENCE_INTERVAL:
            case ScaveModelPackage.COMPUTE_SCALAR__CONFIDENCE_LEVEL:
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_MIN_MAX:
                fireNotifyChanged(new ViewerNotification(notification, notification.getNotifier(), false, true));
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__FILTERS:
                fireNotifyChanged(new ViewerNotification(notification, notification.getNotifier(), true, false));
                return;
        }
        super.notifyChanged(notification);
    }

    /**
     * This adds {@link org.eclipse.emf.edit.command.CommandParameter}s describing the children
     * that can be created under this object.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected void collectNewChildDescriptors(Collection<Object> newChildDescriptors, Object object) {
        super.collectNewChildDescriptors(newChildDescriptors, object);

        newChildDescriptors.add
            (createChildParameter
                (ScaveModelPackage.Literals.COMPUTE_SCALAR__FILTERS,
                 ScaveModelFactory.eINSTANCE.createSelect()));

        newChildDescriptors.add
            (createChildParameter
                (ScaveModelPackage.Literals.COMPUTE_SCALAR__FILTERS,
                 ScaveModelFactory.eINSTANCE.createDeselect()));
    }

}
