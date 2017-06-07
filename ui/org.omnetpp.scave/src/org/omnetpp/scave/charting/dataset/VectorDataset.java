/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.dataset;

import java.util.concurrent.Callable;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.computed.XYVector;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.model2.ResultItemFormatter;
import org.omnetpp.scave.model2.ResultItemValueFormatter;
import org.omnetpp.scave.model2.ScaveModelUtil;
import org.omnetpp.scave.model2.DatasetManager;

/**
 * IXYDataset implementation for output vectors.
 *
 * @author tomi
 */
// TODO bounds should be read from the vector files
public class VectorDataset extends XYDatasetSupport implements IStringValueXYDataset {

    private static class SeriesData
    {
        String key;
        long id;
        Type type;
        InterpolationMode interpolationMode;
        ResultItemValueFormatter formatter;
        XYVector xyarray;
    }

    private String title;
    private String defaultLineTitleFormat;
    private IDList idlist;
    private ResultFileManager manager;
    private SeriesData[] data;

    /**
     * Creates an empty dataset.
     */
    public VectorDataset() {
        this.data = new SeriesData[] {};
    }

    /**
     * Creates a dataset containing vectors from {@code idlist}.
     * The data of the vectors are not loaded/computed, so
     * each series will have 0 items.
     * Intended for accessing the series keys only
     * (property sheet, edit dialog).
     */
    public VectorDataset(String title, IDList idlist, String lineIdFormat, String defaultLineTitleFormat, ResultFileManager manager) { //TODO eliminate this ctor!
        Assert.isLegal(idlist != null);
        Assert.isLegal(manager != null);
        String[] keys = DatasetManager.getResultItemNames(idlist, lineIdFormat, manager);
        Assert.isTrue(idlist.size() == keys.length);
        this.title = title;
        this.defaultLineTitleFormat = defaultLineTitleFormat;
        this.idlist = idlist;
        this.manager = manager;
        this.data = new SeriesData[keys.length];
        for (int i = 0; i < data.length; ++i) {
            long id = idlist.get(i);
            VectorResult vector = manager.getVector(id);
            SeriesData series = data[i] = new SeriesData();
            series.id = id;
            series.key = keys[i];
            series.type = getType(vector);
            series.interpolationMode = getInterpolationMode(vector);
        }
    }

    /**
     * Creates a dataset containing vectors from {@code idlist}.
     * The data of the vectors are loaded/computed.
     * Intended for displaying the dataset (e.g. charts).
     */
    public VectorDataset(String title, IDList idlist, XYVector[] seriesData, String lineIdFormat, String defaultLineTitleFormat, ResultFileManager manager) {
        this(title, idlist, lineIdFormat, defaultLineTitleFormat, manager);
        Assert.isTrue(seriesData != null && data.length == seriesData.length);
        for (int i = 0; i < data.length; ++i) {
            SeriesData series = data[i];
            ResultItem vector = manager.getItem(series.id);
            series.formatter = new ResultItemValueFormatter(vector);
            series.xyarray = seriesData[i];
        }
    }

    public String getTitle(final String format) {
        if (StringUtils.isEmpty(format))
            return title;
        else {
            return ResultFileManager.callWithReadLock(manager, new Callable<String>() {
                public String call() throws Exception {
                    ResultItem[] items = manager == null || ResultItemFormatter.isPlainFormat(format) ?
                            new ResultItem[0] : ScaveModelUtil.getResultItems(idlist, manager);
                            return ResultItemFormatter.formatMultipleResultItem(format, items);
                }
            });
        }
    }

    public int getSeriesCount() {
        return data.length;
    }

    public String getSeriesKey(int series) {
        return data[series].key;
    }

    public String getSeriesTitle(final int series, final String format) {
        return ResultFileManager.callWithReadLock(manager, new Callable<String>() {
            public String call() throws Exception {
                long id = data[series].id;
                ResultItem item = manager.getItem(id);
                return ResultItemFormatter.formatResultItem(format == null ? defaultLineTitleFormat : format, item);
            }
        });
    }

    @Override
    public Type getSeriesType(int series) {
        return data[series].type;
    }

    @Override
    public InterpolationMode getSeriesInterpolationMode(int series) {
        return data[series].interpolationMode;
    }

    public int getItemCount(int series) {
        return data[series].xyarray != null ? data[series].xyarray.size() : 0;
    }

    public double getX(int series, int item) {
        return data[series].xyarray.x[item];
    }

    public BigDecimal getPreciseX(int series, int item) {
        return null; //TODO data[series].xyarray.getPreciseX(item);
    }

    public String getXAsString(int series, int item) {
        BigDecimal xp = getPreciseX(series, item);
        return xp != null ? xp.toString() : String.format("%g", getX(series, item));
    }

    public double getY(int series, int item) {
        return data[series].xyarray.y[item];
    }

    public BigDecimal getPreciseY(int series, int item) {
        return new BigDecimal(getY(series, item));
    }

    public String getYAsString(int series, int item) {
        ResultItemValueFormatter formatter = data[series].formatter;
        return formatter.format(getY(series, item));
    }

    public long getID(int series) {
        return data[series].id;
    }

    private static Type getType(VectorResult vector) {
        ResultItem.DataType type = vector.getDataType();
        if (type == ResultItem.DataType.TYPE_ENUM)
            return Type.Enum;
        else if (type == ResultItem.DataType.TYPE_INT)
            return Type.Int;
        else
            return Type.Double;
    }

    @SuppressWarnings("static-access")
    private static InterpolationMode getInterpolationMode(VectorResult vector) {
        org.omnetpp.scave.engine.InterpolationMode mode = vector.getInterpolationMode();
        if (mode == null || mode == mode.UNSPECIFIED)
            return InterpolationMode.Unspecified;
        else if (mode == mode.NONE)
            return InterpolationMode.None;
        else if (mode == mode.LINEAR)
            return InterpolationMode.Linear;
        else if (mode == mode.SAMPLE_HOLD)
            return InterpolationMode.SampleHold;
        else if (mode == mode.BACKWARD_SAMPLE_HOLD)
            return InterpolationMode.BackwardSampleHold;
        return InterpolationMode.Unspecified;
    }
}
