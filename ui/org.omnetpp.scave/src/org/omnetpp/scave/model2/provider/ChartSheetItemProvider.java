package org.omnetpp.scave.model2.provider;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.provider.ViewerNotification;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.ScaveModelPackage;

public class ChartSheetItemProvider extends
		org.omnetpp.scave.model.provider.ChartSheetItemProvider {

	public ChartSheetItemProvider(AdapterFactory adapterFactory) {
		super(adapterFactory);
	}
	
	/**
	 * Override default implementation to wrap the values of the 
	 * 'charts' reference.
	 */
	protected boolean isWrappingNeeded(Object object) {
		return true;
	}

	@Override
	public void notifyChanged(Notification notification) {
		updateChildren(notification);

		switch (notification.getFeatureID(ChartSheet.class)) {
			case ScaveModelPackage.CHART_SHEET__NAME:
				fireNotifyChanged(new ViewerNotification(notification, notification.getNotifier(), false, true));
				return;
			case ScaveModelPackage.CHART_SHEET__CHARTS:
				// Set labelUpdate to true, because the number of the charts is
				// displayed in the label of the ChartSheet
				fireNotifyChanged(new ViewerNotification(notification, notification.getNotifier(), true, true));
				return;
		}
	}
}
