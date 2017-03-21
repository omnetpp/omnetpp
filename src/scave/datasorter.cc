//==========================================================================
//  DATASORTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "resultfilemanager.h"
#include "datasorter.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

ResultFileManager *DataSorter::tmpResultFileMgr;

/*----------------------------------------
 *              XYDataset
 *----------------------------------------*/
void XYDataset::add(const ScalarResult& d)
{
    int row, column;

    KeyToIndexMap::iterator rowRef = rowKeyToIndexMap.find(d);
    if (rowRef != rowKeyToIndexMap.end()) {
        row = rowOrder[rowRef->second];
    }
    else {  // add new row
        row = rowKeys.size();
        values.push_back(vector<Statistics>(columnKeys.size(), Statistics()));
        rowKeyToIndexMap[d] = row;
        rowKeys.push_back(d);
        rowOrder.push_back(row);
    }

    KeyToIndexMap::iterator columnRef = columnKeyToIndexMap.find(d);
    if (columnRef != columnKeyToIndexMap.end()) {
        column = columnOrder[columnRef->second];
    }
    else {  // add new column
        column = columnKeys.size();
        for (vector<Row>::iterator rowRef = values.begin(); rowRef != values.end(); ++rowRef)
            rowRef->push_back(Statistics());
        columnKeyToIndexMap[d] = column;
        columnKeys.push_back(d);
        columnOrder.push_back(column);
    }

    values.at(row).at(column).collect(d.getValue());
}

void XYDataset::swapRows(int row1, int row2)
{
    int temp = rowOrder[row1];
    rowOrder[row1] = rowOrder[row2];
    rowOrder[row2] = temp;
}

void XYDataset::sortRows()
{
    vector<int> rowOrder;
    for (KeyToIndexMap::iterator it = rowKeyToIndexMap.begin(); it != rowKeyToIndexMap.end(); ++it)
        rowOrder.push_back(this->rowOrder[it->second]);

    this->rowOrder = rowOrder;
}

void XYDataset::sortColumns()
{
    vector<int> columnOrder;
    for (KeyToIndexMap::iterator it = columnKeyToIndexMap.begin(); it != columnKeyToIndexMap.end(); ++it)
        columnOrder.push_back(this->columnOrder[it->second]);
    this->columnOrder = columnOrder;
}

struct ValueAndIndex
{
    double value;
    int index;
    ValueAndIndex(double value, int index) : value(value), index(index) {}
    bool operator<(const ValueAndIndex& other) const { return this->value < other.value; }
};

void XYDataset::sortColumnsAccordingToFirstRowMean()
{
    if (!values.empty()) {
        vector<ValueAndIndex> vals;
        int firstRow = rowOrder[0];
        for (int i = 0; i < (int)values[firstRow].size(); ++i) {
            double mean = values[firstRow][i].getMean();
            if (!isNaN(mean))
                vals.push_back(ValueAndIndex(mean, i));
        }

        sort(vals.begin(), vals.end());

        columnOrder = vector<int>(vals.size());
        for (int i = 0; i < (int)vals.size(); ++i)
            columnOrder[i] = vals[i].index;
    }
}

std::string XYDataset::getRowField(int row, ResultItemField field) const
{
    if (row < 0 || row >= getRowCount() || !rowFields.hasField(field))
        return "";
    return field.getFieldValue(rowKeys[rowOrder[row]]);
}

std::string XYDataset::getRowFieldNoCheck(int row, ResultItemField field) const
{
    if (row < 0 || row >= getRowCount())
        return "";
    return field.getFieldValue(rowKeys[rowOrder[row]]);
}

std::string XYDataset::getColumnField(int column, ResultItemField field) const
{
    if (column < 0 || column >= getColumnCount() || !columnFields.hasField(field))
        return "";
    return field.getFieldValue(columnKeys[columnOrder[column]]);
}

const Statistics& XYDataset::getValue(int row, int column) const
{
    static Statistics empty;
    if (row < 0 || column < 0 || row >= getRowCount() || column >= getColumnCount())
        return empty;
    return values.at(rowOrder[row]).at(columnOrder[column]);
}

/*
 * Grouping functions
 */
bool DataSorter::sameGroupFileRunScalar(const ResultItem& d1, const ResultItem& d2)
{
    return d1.getFileRun() == d2.getFileRun() && d1.getName() == d2.getName();
}

bool DataSorter::sameGroupModuleScalar(const ResultItem& d1, const ResultItem& d2)
{
    return d1.getModuleName() == d2.getModuleName() && d1.getName() == d2.getName();
}

bool DataSorter::sameGroupFileRunModule(const ResultItem& d1, const ResultItem& d2)
{
    return d1.getFileRun() == d2.getFileRun() && d1.getModuleName() == d2.getModuleName();
}

/*
 * Compare functions
 */
bool DataSorter::lessByModuleRef(ID id1, ID id2)
{
    if (id1 == -1 || id2 == -1)
        return id2 != -1;  // -1 is the smallest
    const ResultItem& d1 = tmpResultFileMgr->getItem(id1);
    const ResultItem& d2 = tmpResultFileMgr->getItem(id2);
    return strdictcmp(d1.getModuleName().c_str(), d2.getModuleName().c_str()) < 0;
}

bool DataSorter::equalByModuleRef(ID id1, ID id2)
{
    if (id1 == -1 || id2 == -1)
        return id1 == id2;
    const ResultItem& d1 = tmpResultFileMgr->getItem(id1);
    const ResultItem& d2 = tmpResultFileMgr->getItem(id2);
    return d1.getModuleName() == d2.getModuleName();
}

bool DataSorter::lessByFileAndRun(ID id1, ID id2)
{
    if (id1 == -1 || id2 == -1)
        return id2 != -1;  // -1 is the smallest
    const ResultItem& d1 = tmpResultFileMgr->getItem(id1);
    const ResultItem& d2 = tmpResultFileMgr->getItem(id2);

    // compare first by file, then by run
    int cmpFile = strdictcmp(d1.getFile()->getFilePath().c_str(), d2.getFile()->getFilePath().c_str());
    if (cmpFile != 0)
        return cmpFile < 0;
    int cmpRun = strdictcmp(d1.getRun()->getRunName().c_str(), d2.getRun()->getRunName().c_str());
    return cmpRun < 0;
}

bool DataSorter::equalByFileAndRun(ID id1, ID id2)
{
    if (id1 == -1 || id2 == -1)
        return id1 == id2;
    const ResultItem& d1 = tmpResultFileMgr->getItem(id1);
    const ResultItem& d2 = tmpResultFileMgr->getItem(id2);
    return d1.getFileRun() == d2.getFileRun();
}

bool DataSorter::lessByName(ID id1, ID id2)
{
    if (id1 == -1 || id2 == -1)
        return id2 != -1;  // -1 is the smallest
    const ResultItem& d1 = tmpResultFileMgr->getItem(id1);
    const ResultItem& d2 = tmpResultFileMgr->getItem(id2);
    return strdictcmp(d1.getName().c_str(), d2.getName().c_str()) < 0;
}

bool DataSorter::equalByName(ID id1, ID id2)
{
    if (id1 == -1 || id2 == -1)
        return id1 == id2;
    const ResultItem& d1 = tmpResultFileMgr->getItem(id1);
    const ResultItem& d2 = tmpResultFileMgr->getItem(id2);
    return d1.getName() == d2.getName();
}

bool DataSorter::lessByValue(ID id1, ID id2)
{
    if (id1 == -1 || id2 == -1)
        return id2 != -1;  // -1 is the smallest
    const ScalarResult& d1 = tmpResultFileMgr->getScalar(id1);
    const ScalarResult& d2 = tmpResultFileMgr->getScalar(id2);
    return d1.getValue() < d2.getValue();
}

/*----------------------------------------
 *             DataSorter
 *----------------------------------------*/

template<class GroupingFn>
IDVectorVector DataSorter::doGrouping(const IDList& idlist, GroupingFn sameGroup)
{
    tmpResultFileMgr = resultFileMgr;

    // parse idlist and do grouping as well, on the fly
    IDVectorVector vv;
    int sz = idlist.size();
    for (int ii = 0; ii < sz; ii++) {
        ID id = idlist.get(ii);

        // check of this id shares fileRef, runNumber & scalarName with one of the
        // IDVectors already in vv
        const ResultItem& d = resultFileMgr->getItem(id);
        IDVectorVector::iterator i;
        for (i = vv.begin(); i != vv.end(); ++i) {
            ID vvid = (*i)[0];  // first element in IDVector selected by i
            const ResultItem& vvd = resultFileMgr->getItem(vvid);
            if (sameGroup(d, vvd))
                break;
        }
        if (i == vv.end()) {
            // not found -- new one has to be added
            vv.push_back(IDVector());
            i = vv.end()-1;
        }

        // insert
        i->push_back(id);
    }
    return vv;
}

template<class LessFn, class EqualFn>
void DataSorter::sortAndAlign(IDVectorVector& vv, LessFn less, EqualFn equal)
{
    tmpResultFileMgr = resultFileMgr;

    // order each group
    for (IDVectorVector::iterator i = vv.begin(); i != vv.end(); ++i)
        std::sort(i->begin(), i->end(), less);

    // now insert "null" elements (id=-1) so that every group is of same length,
    // and same indices are "equal()"
    for (int pos = 0; ; pos++) {
        // determine "smallest" element in all vectors, on position "pos"
        ID minId = -1;
        IDVectorVector::iterator i;
        for (i = vv.begin(); i != vv.end(); ++i)
            if ((int)i->size() > pos)
                minId = (minId == -1) ? (*i)[pos] : less((*i)[pos], minId) ? (*i)[pos] : minId;


        // if pos is past the end of all vectors, we're done
        if (minId == -1)
            break;

        // if a vector has something different on this position, add a "null" element here
        for (i = vv.begin(); i != vv.end(); ++i)
            if ((int)i->size() <= pos || !equal((*i)[pos], minId))
                i->insert(i->begin()+pos, -1);

    }
}

IDVectorVector DataSorter::groupByFields(const IDList& idlist, const ResultItemFields& fields)
{
    return doGrouping(idlist, ResultItemFieldsEqual(fields));
}

IDVectorVector DataSorter::groupAndAlign(const IDList& idlist, const ResultItemFields& fields)
{
    IDVectorVector vv = doGrouping(idlist, ResultItemFieldsEqual(fields));

    sortAndAlign(vv,
            IDFieldsLess(fields.complement(), resultFileMgr),
            IDFieldsEqual(fields.complement(), resultFileMgr));
    return vv;
}

/*
 * Returns true iff the jth column is missing
 * i.e. X value (i=0) is missing or each Y value (i>0) is missing.
 */
static bool isMissing(const IDVectorVector& vv, int j)
{
    if (vv[0][j] == -1)
        return true;
    bool foundY = false;
    for (int i = 1; i < (int)vv.size(); ++i)
        if (vv[i][j] != -1) {
            foundY = true;
            break;
        }
    return !foundY;
}

XYDataset DataSorter::groupAndAggregate(const IDList& scalars, const ResultItemFields& rowFields, const ResultItemFields& columnFields)
{
    Assert(scalars.areAllScalars());

    XYDataset dataset(rowFields, columnFields);

    int sz = scalars.size();
    for (int i = 0; i < sz; i++) {
        ID id = scalars.get(i);
        const ScalarResult& d = resultFileMgr->getScalar(id);
        dataset.add(d);
    }
    return dataset;
}

IDVectorVector DataSorter::prepareScatterPlot(const IDList& scalars, const char *moduleName, const char *scalarName)
{
    Assert(scalars.areAllScalars());

    // form groups (IDVectors) by moduleName+scalarName
    IDVectorVector vv = doGrouping(scalars, sameGroupModuleScalar);
    if (vv.empty())
        return vv;

    // order each group by fileRef+runNumber, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same fileRef+runNumber
    sortAndAlign(vv, lessByFileAndRun, equalByFileAndRun);

    // find series for X axis (modulename, scalarname)...
    int xpos = -1;
    for (IDVectorVector::iterator i = vv.begin(); i != vv.end(); ++i) {
        ID id = -1;
        for (IDVector::iterator j = i->begin(); j != i->end(); ++j)
            if (*j != -1) {
                id = *j;
                break;
            }
        if (id == -1)
            continue;
        const ScalarResult& d = resultFileMgr->getScalar(id);
        if (d.getModuleName() == moduleName && d.getName() == scalarName) {
            xpos = i-vv.begin();
            break;
        }
    }
    if (xpos == -1)
        throw opp_runtime_error("Data for X axis not found");

    // ... and bring X series to 1st place
    if (xpos != 0)
        std::swap(vv[0], vv[xpos]);

    // sort x axis, moving elements in all other vectors as well.
    // Strategy: we'll construct the result in vv2. First we sort X axis, then
    // move elements of the other vectors to the same positions where the
    // X values went.

    IDVectorVector vv2;
    vv2.resize(vv.size());
    vv2[0] = vv[0];

    // step one: remove values where X value is missing or each Y value is missing (id=-1)
    IDVector::iterator it = vv2[0].begin();
    int sz = vv[0].size();
    for (int j = 0; j < sz; ++j) {
        if (isMissing(vv, j))
            vv2[0].erase(it);
        else
            ++it;
    }

    // step two: sort X axis
    std::sort(vv2[0].begin(), vv2[0].end(), lessByValue);

    // step three: allocate all other vectors in vv2 to be the same length
    // (necessary because we'll fill them in via assignment, NOT push_back() or insert())
    for (int k = 1; k < (int)vv2.size(); k++)
        vv2[k].resize(vv2[0].size());

    // step four: copy over elements
    for (int pos = 0; pos < (int)vv[0].size(); pos++) {
        ID id = vv[0][pos];
        if (id == -1)
            continue;
        IDVector::iterator dest = std::find(vv2[0].begin(), vv2[0].end(), id);
        if (dest == vv2[0].end())
            continue;
        int destpos = dest - vv2[0].begin();
        for (int k = 1; k < (int)vv.size(); k++)
            vv2[k][destpos] = vv[k][pos];
    }

    return vv2;
}

XYDataset DataSorter::prepareScatterPlot2(const IDList& scalars, const char *moduleName, const char *scalarName,
        const ResultItemFields& rowFields, const ResultItemFields& columnFields)
{
    Assert(scalars.areAllScalars());

    XYDataset dataset = groupAndAggregate(scalars, rowFields, columnFields);

    // find row of x values and move it to row 0
    int row;
    for (row = 0; row < dataset.getRowCount(); ++row) {
        std::string moduleName = dataset.getRowField(row, ResultItemField(ResultItemField::MODULE));
        std::string dataName = dataset.getRowField(row, ResultItemField(ResultItemField::NAME));
        if (dataset.getRowField(row, ResultItemField(ResultItemField::MODULE)) == moduleName &&
            dataset.getRowField(row, ResultItemField(ResultItemField::NAME)) == scalarName)
            break;
    }
    if (row < dataset.getRowCount()) {
        if (row > 0)
            dataset.swapRows(0, row);
    }
    else
        throw opp_runtime_error("Data for X axis not found.");

    // sort columns so that X values are in ascending order
    dataset.sortColumnsAccordingToFirstRowMean();

    return dataset;
}

struct IsoGroupingFn : public std::binary_function<ResultItem, ResultItem, bool>
{
    typedef map<Run *, vector<double> > RunIsoValueMap;
    typedef map<pair<string, string>, int> IsoAttrIndexMap;
    RunIsoValueMap isoMap;
    ResultItemFields fields;

    IsoGroupingFn() {}
    IDList init(const IDList& idlist, const StringVector& moduleNames, const StringVector& scalarNames,
            const ResultItemFields& fields, ResultFileManager *manager);
    bool operator()(const ResultItem& d1, const ResultItem& d2) const;
};

IDList IsoGroupingFn::init(const IDList& idlist, const StringVector& moduleNames, const StringVector& scalarNames,
        const ResultItemFields& fields, ResultFileManager *manager)
{
    this->fields = fields;

    // assert(moduleNames.size() == scalarNames.size());
    int numOfIsoValues = scalarNames.size();
    IDList result;

    // build iso (module,scalar) -> index map
    IsoAttrIndexMap indexMap;
    for (int i = 0; i < numOfIsoValues; ++i) {
        pair<string, string> key = make_pair(moduleNames[i], scalarNames[i]);
        indexMap[key] = i;
    }

    // build run -> iso values map
    int sz = idlist.size();
    for (int i = 0; i < sz; ++i) {
        ID id = idlist.get(i);
        const ScalarResult& scalar = manager->getScalar(id);
        pair<string, string> key = make_pair(scalar.getModuleName(), scalar.getName());
        IsoAttrIndexMap::iterator it = indexMap.find(key);
        if (it != indexMap.end()) {
            int index = it->second;
            Run *run = scalar.getRun();
            RunIsoValueMap::iterator it2 = isoMap.find(run);
            if (it2 == isoMap.end()) {
                it2 = isoMap.insert(make_pair(run, vector<double>(numOfIsoValues, NaN))).first;
            }
            it2->second[index] = scalar.getValue();
        }
        else
            result.add(id);
    }

    return result;
}

bool IsoGroupingFn::operator()(const ResultItem& d1, const ResultItem& d2) const
{
    if (d1.getRun() == d2.getRun())
        return true;

    if (!fields.equal(d1, d2))
        return false;

    if (isoMap.empty())
        return true;

    RunIsoValueMap::const_iterator it1 = isoMap.find(d1.getRun());
    RunIsoValueMap::const_iterator it2 = isoMap.find(d2.getRun());
    if (it1 == isoMap.end() || it2 == isoMap.end())
        return false;

    const vector<double>& v1 = it1->second;
    const vector<double>& v2 = it2->second;

    assert(v1.size() == v2.size());

    for (int i = 0; i < (int)v1.size(); ++i)
        if (v1[i] != v2[i])
            return false;


    return true;
}

XYDatasetVector DataSorter::prepareScatterPlot3(const IDList& scalars, const char *moduleName, const char *scalarName,
        const ResultItemFields& rowFields, const ResultItemFields& columnFields,
        const StringVector& isoModuleNames, const StringVector& isoScalarNames, const ResultItemFields& isoFields)
{
    Assert(scalars.areAllScalars());

    // group data according to iso fields
    IsoGroupingFn grouping;
    IDList nonIsoScalars = grouping.init(scalars, isoModuleNames, isoScalarNames, isoFields, resultFileMgr);
    IDVectorVector groupedScalars = doGrouping(nonIsoScalars, grouping);

    XYDatasetVector datasets;
    for (IDVectorVector::iterator group = groupedScalars.begin(); group != groupedScalars.end(); ++group) {
        IDList groupAsIDList;
        for (IDVector::iterator id = group->begin(); id != group->end(); id++)
            groupAsIDList.add(*id);

        try {
            XYDataset dataset = prepareScatterPlot2(groupAsIDList, moduleName, scalarName, rowFields, columnFields);
            datasets.push_back(dataset);
        }
        catch (opp_runtime_error& e) {
            // no X data for some iso values -> omit from the result
        }
    }

    return datasets;
}

IDList DataSorter::getModuleAndNamePairs(const IDList& idlist, int maxcount)
{
    IDList out;

    // go through idlist and pick ids that represent a new (module, name pair)
    for (int ii = 0; ii < (int)idlist.size(); ii++) {
        ID id = idlist.get(ii);

        // check if module and name of this id is already in out[]
        const ResultItem& d = resultFileMgr->getItem(id);
        int i;
        int outSize = out.size();
        for (i = 0; i < outSize; i++) {
            const ResultItem& vd = resultFileMgr->getItem(out.get(i));
            if (d.getModuleName() == vd.getModuleName() && d.getName() == vd.getName())
                break;
        }

        // not yet -- then add it
        if (i == outSize) {
            out.add(id);
            if (outSize > maxcount)
                break;  // enough is enough
        }
    }

    return out;
}

IDVectorVector DataSorter::prepareCopyToClipboard(const IDList& idlist)
{
    // form groups (IDVectors) by fileRef+runNumber+moduleNameRef
    IDVectorVector vv = doGrouping(idlist, sameGroupFileRunModule);

    // order each group by scalar name, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same scalarNameRefs
    sortAndAlign(vv, lessByName, equalByName);

    return vv;
}

}  // namespace scave
}  // namespace omnetpp

