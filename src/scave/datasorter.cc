//==========================================================================
//  DATASORTER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "resultfilemanager.h"
#include "commonutil.h"
#include "datasorter.h"
#include "stringutil.h"

using namespace std;


ResultFileManager *ScalarDataSorter::tmpScalarMgr;

/*----------------------------------------
 *              ResultItemFields
 *----------------------------------------*/

StringVector ResultItemFields::getFieldNames()
{
    StringVector names = StringVector();
    names.push_back("file");
    names.push_back("run");
    names.push_back("module");
    names.push_back("name");
    names.push_back("experiment");
    names.push_back("measurement");
    names.push_back("replication");
    return names;
}

ResultItemFields::ResultItemFields(const StringVector &fieldNames)
{
    fields = 0;
    for (StringVector::const_iterator it = fieldNames.begin(); it != fieldNames.end(); ++it)
    {
        if (*it == "file") fields |= FILE;
        else if (*it == "run") fields |= RUN;
        else if (*it == "module") fields |= MODULE;
        else if (*it == "name") fields |= NAME;
        else if (*it == "experiment") fields |= EXPERIMENT;
        else if (*it == "measurement") fields |= MEASUREMENT;
        else if (*it == "replication") fields |= REPLICATION;
    }
}

static int strcmp_safe(const char *s1, const char *s2)
{
    if (s1 == NULL || s2 == NULL)
        return (s1 == NULL && s2 == NULL) ? 0 : (s1 == NULL ? 1 : -1);
    return strcmp(s1, s2);
}

inline const char *getAttribute(const ResultItem &d, const char *attrName)
{
    return d.fileRunRef->runRef->getAttribute(attrName);
}

bool ResultItemFields::equal(ID id1, ID id2, ResultFileManager *manager)
{
    if (id1==-1 || id2==-1) return id1==id2;
    const ResultItem& d1 = manager->getItem(id1);
    const ResultItem& d2 = manager->getItem(id2);
    return equal(d1, d2);
}

bool ResultItemFields::equal(const ResultItem& d1, const ResultItem& d2)
{
    if (hasField(FILE) && d1.fileRunRef->fileRef != d2.fileRunRef->fileRef) return false;
    if (hasField(RUN) && d1.fileRunRef->runRef != d2.fileRunRef->runRef) return false;
    if (hasField(MODULE) && d1.moduleNameRef != d2.moduleNameRef) return false;
    if (hasField(NAME) && d1.nameRef != d2.nameRef) return false;
    if (hasField(EXPERIMENT) && strcmp_safe(getAttribute(d1, "experiment"), getAttribute(d2, "experiment")) != 0) return false;
    if (hasField(MEASUREMENT) && strcmp_safe(getAttribute(d1, "measurement"), getAttribute(d2, "measurement")) != 0) return false;
    if (hasField(REPLICATION) && strcmp_safe(getAttribute(d1, "replication"), getAttribute(d2, "replication")) != 0) return false;
    return true;
}

bool ResultItemFields::less(ID id1, ID id2, ResultFileManager *manager)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ResultItem& d1 = manager->getItem(id1);
    const ResultItem& d2 = manager->getItem(id2);
    return less(d1, d2);
}

bool ResultItemFields::less(const ResultItem &d1, const ResultItem &d2) const
{
    int cmp;

#define CMP(field,cmp_fun, f1,f2)   if (hasField(field)) { cmp = cmp_fun(f1,f2); if (cmp) return cmp < 0; }

    CMP(FILE, strdictcmp, d1.fileRunRef->runRef->runName.c_str(), d2.fileRunRef->runRef->runName.c_str());
    CMP(RUN, strdictcmp, d1.fileRunRef->runRef->runName.c_str(), d2.fileRunRef->runRef->runName.c_str());
    CMP(MODULE, strdictcmp, d1.moduleNameRef->c_str(), d2.moduleNameRef->c_str());
    CMP(NAME, strdictcmp, d1.nameRef->c_str(), d2.nameRef->c_str());
    CMP(EXPERIMENT, strcmp_safe, getAttribute(d1, "experiment"), getAttribute(d2, "experiment"));
    CMP(MEASUREMENT, strcmp_safe, getAttribute(d1, "measurement"), getAttribute(d2, "measurement"));
    CMP(REPLICATION, strcmp_safe, getAttribute(d1, "replication"), getAttribute(d2, "replication"));

    return false; // ==
}

string ResultItemFields::getField(const ResultItem& d)
{
    if (fields == FILE)
        return d.fileRunRef->fileRef->filePath;
    else if (fields == RUN)
        return d.fileRunRef->runRef->runName;
    else if (fields == MODULE)
        return *d.moduleNameRef;
    else if (fields == NAME)
        return *d.nameRef;
    else if (fields == EXPERIMENT) {
        const char *value = getAttribute(d, "experiment");
        return value != NULL ? value : "";
    }
    else if (fields == MEASUREMENT) {
        const char *value = getAttribute(d, "measurement");
        return value != NULL ? value : "";
    }
    else if (fields == REPLICATION) {
        const char *value = getAttribute(d, "replication");
        return value != NULL ? value : "";
    }
    else
        return "";
}

/*----------------------------------------
 *              XYDataset
 *----------------------------------------*/
void XYDataset::add(const ScalarResult &d)
{
    int row, column;

    KeyToIndexMap::iterator rowRef = rowKeyToIndexMap.find(d);
    if (rowRef != rowKeyToIndexMap.end())
    {
        row = rowOrder[rowRef->second];
    }
    else // add new row
    {
        row = rowKeys.size();
        values.push_back(vector<Mean>(columnKeys.size(), Mean()));
        rowKeyToIndexMap[d] = row;
        rowKeys.push_back(d);
        rowOrder.push_back(row);
    }

    KeyToIndexMap::iterator columnRef = columnKeyToIndexMap.find(d);
    if (columnRef != columnKeyToIndexMap.end())
    {
        column = columnOrder[columnRef->second];
    }
    else // add new column
    {
        column = columnKeys.size();
        for (vector<Row>::iterator rowRef = values.begin(); rowRef != values.end(); ++rowRef)
            rowRef->push_back(Mean());
        columnKeyToIndexMap[d] = column;
        columnKeys.push_back(d);
        columnOrder.push_back(column);
    }

    values.at(row).at(column).accumulate(d.value);
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
    bool operator<(const ValueAndIndex& other) { return this->value < other.value; }
};

void XYDataset::sortColumnsAccordingToFirstRow()
{
    if (values.size() > 0)
    {
        vector<ValueAndIndex> vals;
        for (int i = 0; i < values[0].size(); ++i)
        {
            if (!values[0][i].isNaN())
                vals.push_back(ValueAndIndex(values[0][i].value(), i));
        }

        sort(vals.begin(), vals.end());

        columnOrder = vector<int>(vals.size());
        for (int i = 0; i < vals.size(); ++i)
            columnOrder[i] = vals[i].index;
    }
}

/*
 * Grouping functions
 */
template<class T>
class MemberGroupingFunc
{
    private:
        typedef bool (T::*GroupingFunc)(const ResultItem&, const ResultItem&);
        T &object;
        GroupingFunc func;
    public:
        MemberGroupingFunc(T &object, GroupingFunc func) : object(object), func(func) {}
        bool operator()(const ResultItem& d1, const ResultItem& d2) { return (object.*func)(d1, d2); }
};

bool ScalarDataSorter::sameGroupFileRunScalar(const ScalarResult& d1, const ScalarResult& d2)
{
    return d1.fileRunRef==d2.fileRunRef && d1.nameRef==d2.nameRef;
}

bool ScalarDataSorter::sameGroupModuleScalar(const ScalarResult& d1, const ScalarResult& d2)
{
    return d1.moduleNameRef==d2.moduleNameRef && d1.nameRef==d2.nameRef;
}

bool ScalarDataSorter::sameGroupFileRunModule(const ScalarResult& d1, const ScalarResult& d2)
{
    return d1.fileRunRef==d2.fileRunRef && d1.moduleNameRef==d2.moduleNameRef;
}

/*
 * Compare functions
 */
template<class T>
class MemberCompareFunc
{
    private:
        typedef bool (T::*CompareFunc)(ID id1, ID id2, ResultFileManager *manager);
        ResultFileManager *manager;
        T &object;
        CompareFunc func;
    public:
        MemberCompareFunc(T &object, CompareFunc func, ResultFileManager *manager)
            : object(object), func(func), manager(manager) {}
        bool operator()(ID id1, ID id2) { return (object.*func)(id1, id2, manager); }
};

bool ScalarDataSorter::lessByModuleRef(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);
    return strdictcmp(d1.moduleNameRef->c_str(), d2.moduleNameRef->c_str()) < 0;
}

bool ScalarDataSorter::equalByModuleRef(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id1==id2;
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);
    return d1.moduleNameRef == d2.moduleNameRef;
}

bool ScalarDataSorter::lessByFileAndRun(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);

    // compare first by file, then by run
    int cmpFile = strdictcmp(d1.fileRunRef->fileRef->filePath.c_str(), d2.fileRunRef->fileRef->filePath.c_str());
    if (cmpFile!=0)
        return cmpFile < 0;
    int cmpRun = strdictcmp(d1.fileRunRef->runRef->runName.c_str(), d2.fileRunRef->runRef->runName.c_str());
    return cmpRun < 0;
}

bool ScalarDataSorter::equalByFileAndRun(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id1==id2;
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);
    return d1.fileRunRef == d2.fileRunRef;
}

bool ScalarDataSorter::lessByScalarNameRef(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);
    return strdictcmp(d1.nameRef->c_str(), d2.nameRef->c_str()) < 0;
}

bool ScalarDataSorter::equalByScalarNameRef(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id1==id2;
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);
    return d1.nameRef == d2.nameRef;
}

bool ScalarDataSorter::lessByValue(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);
    return d1.value < d2.value;
}

/*----------------------------------------
 *             ScalarDataSorter
 *----------------------------------------*/

template<class GroupingFn>
IDVectorVector ScalarDataSorter::doGrouping(const IDList& idlist, GroupingFn sameGroup)
{
    tmpScalarMgr = resultFileMgr;

    // parse idlist and do grouping as well, on the fly
    IDVectorVector vv;
    int sz = idlist.size();
    for (int ii = 0; ii < sz; ii++)
    {
        ID id = idlist.get(ii);

        // check of this id shares fileRef, runNumber & scalarName with one of the
        // IDVectors already in vv
        const ScalarResult& d = resultFileMgr->getScalar(id);
        IDVectorVector::iterator i;
        for (i=vv.begin(); i!=vv.end(); ++i)
        {
            ID vvid = (*i)[0];  // first element in IDVector selected by i
            const ScalarResult& vvd = resultFileMgr->getScalar(vvid);
            if (sameGroup(d,vvd))
                break;
        }
        if (i==vv.end())
        {
            // not found -- new one has to be added
            vv.push_back(IDVector());
            i = vv.end()-1;
        }

        // insert
        i->push_back(id);
    }
    return vv;
}

template <class LessFn, class EqualFn>
void ScalarDataSorter::sortAndAlign(IDVectorVector& vv, LessFn less, EqualFn equal)
{
    tmpScalarMgr = resultFileMgr;

    // order each group
    for (IDVectorVector::iterator i=vv.begin(); i!=vv.end(); ++i)
        std::sort(i->begin(), i->end(), less);

    // now insert "null" elements (id=-1) so that every group is of same length,
    // and same indices are "equal()"
    for (int pos=0; ; pos++)
    {
        // determine "smallest" element in all vectors, on position "pos"
        ID minId = -1;
        IDVectorVector::iterator i;
        for (i=vv.begin(); i!=vv.end(); ++i)
            if ((int)i->size()>pos)
                minId = (minId==-1) ? (*i)[pos] : less((*i)[pos],minId) ? (*i)[pos] : minId;

        // if pos is past the end of all vectors, we're done
        if (minId==-1)
            break;

        // if a vector has something different on this position, add a "null" element here
        for (i=vv.begin(); i!=vv.end(); ++i)
            if ((int)i->size()<=pos || !equal((*i)[pos],minId))
                i->insert(i->begin()+pos,-1);
    }
}

IDVectorVector ScalarDataSorter::groupByRunAndName(const IDList& idlist)
{
    // form groups (IDVectors) by fileRef+runNumber+scalarName
    IDVectorVector vv = doGrouping(idlist, sameGroupFileRunScalar);

    // order each group by module name, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same moduleNameRefs
    sortAndAlign(vv, lessByModuleRef, equalByModuleRef);

    return vv;
}

IDVectorVector ScalarDataSorter::groupByModuleAndName(const IDList& idlist)
{
    // form groups (IDVectors) by moduleName+scalarName
    IDVectorVector vv = doGrouping(idlist, sameGroupModuleScalar);

    // order each group by fileRef+runNumber, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same fileRef+runNumber
    sortAndAlign(vv, lessByFileAndRun, equalByFileAndRun);

    return vv;
}

IDVectorVector ScalarDataSorter::groupByFields(const IDList& idlist, ResultItemFields fields)
{
    IDVectorVector vv = doGrouping(idlist, MemberGroupingFunc<ResultItemFields>(fields, &ResultItemFields::equal));

    sortAndAlign(vv,
        MemberCompareFunc<ResultItemFields>(fields.complement(), &ResultItemFields::less, resultFileMgr),
        MemberCompareFunc<ResultItemFields>(fields.complement(), &ResultItemFields::equal, resultFileMgr));
    return vv;
}

/*
 * Returns true iff the jth column is missing
 * i.e. X value (i=0) is missing or each Y value (i>0) is missing.
 */
static bool isMissing(const IDVectorVector &vv, int j)
{
    if (vv[0][j]==-1)
        return true;
    bool foundY = false;
    for (int i=1; i<vv.size();++i)
        if (vv[i][j]!=-1)
        {
            foundY = true;
            break;
        }
    return !foundY;
}

XYDataset ScalarDataSorter::groupAndAggregate(const IDList& idlist, ResultItemFields rowFields, ResultItemFields columnFields)
{
    XYDataset dataset(rowFields, columnFields);

    int sz = idlist.size();
    for (int i = 0; i < sz; i++)
    {
        ID id = idlist.get(i);
        const ScalarResult& d = resultFileMgr->getScalar(id);
        dataset.add(d);
    }
    return dataset;
}


IDVectorVector ScalarDataSorter::prepareScatterPlot(const IDList& idlist, const char *moduleName, const char *scalarName)
{
    // form groups (IDVectors) by moduleName+scalarName
    IDVectorVector vv = doGrouping(idlist, sameGroupModuleScalar);
    if (vv.size()==0)
        return vv;

    // order each group by fileRef+runNumber, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same fileRef+runNumber
    sortAndAlign(vv, lessByFileAndRun, equalByFileAndRun);

    // find series for X axis (modulename, scalarname)...
    int xpos = -1;
    for (IDVectorVector::iterator i=vv.begin(); i!=vv.end(); ++i)
    {
        ID id = -1;
        for (IDVector::iterator j=i->begin(); j!=i->end(); ++j)
            if (*j!=-1)
                {id = *j;break;}
        if (id==-1)
            continue;
        const ScalarResult& d = resultFileMgr->getScalar(id);
        if (*d.moduleNameRef==moduleName && *d.nameRef==scalarName)
            {xpos = i-vv.begin();break;}
    }
    if (xpos==-1)
        throw opp_runtime_error("data for X axis not found");

    // ... and bring X series to 1st place
    if (xpos!=0)
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
    for (int j=0; j<sz; ++j)
    {
        if(isMissing(vv,j))
            vv2[0].erase(it);
        else
            ++it;
    }

    // step two: sort X axis
    std::sort(vv2[0].begin(), vv2[0].end(), lessByValue);

    // step three: allocate all other vectors in vv2 to be the same length
    // (necessary because we'll fill them in via assignment, NOT push_back() or insert())
    for (int k=1; k<(int)vv2.size(); k++)
        vv2[k].resize(vv2[0].size());

    // step four: copy over elements
    for (int pos=0; pos<(int)vv[0].size(); pos++)
    {
        ID id = vv[0][pos];
        if (id==-1) continue;
        IDVector::iterator dest = std::find(vv2[0].begin(),vv2[0].end(),id);
        if (dest==vv2[0].end()) continue;
        int destpos = dest - vv2[0].begin();
        for (int k=1; k<(int)vv.size(); k++)
            vv2[k][destpos] = vv[k][pos];
    }

    return vv2;
}

XYDataset ScalarDataSorter::prepareScatterPlot2(const IDList& idlist, const char *moduleName, const char *scalarName,
                                                ResultItemFields rowFields, ResultItemFields columnFields)
{
    XYDataset dataset = groupAndAggregate(idlist, rowFields, columnFields);
    
    // find row of x values and move it to row 0
    int row;
    for (row = 0; row < dataset.getRowCount(); ++row)
    {
        std::string moduleName = dataset.getRowField(row, ResultItemFields::MODULE);
        std::string dataName = dataset.getRowField(row, ResultItemFields::NAME);
        if (dataset.getRowField(row, ResultItemFields::MODULE) == moduleName &&
            dataset.getRowField(row, ResultItemFields::NAME) == scalarName)
            break;
    }
    if (row < dataset.getRowCount())
    {
        if (row > 0)
            dataset.swapRows(0, row);
    }
    else
        throw opp_runtime_error("Data for X axis not found.");
    
    // sort columns so that X values are in ascending order
    dataset.sortColumnsAccordingToFirstRow();

    return dataset;
}


IDList ScalarDataSorter::getModuleAndNamePairs(const IDList& idlist, int maxcount)
{
    IDList out;

    // go through idlist and pick ids that represent a new (module, name pair)
    for (int ii = 0; ii < (int)idlist.size(); ii++)
    {
        ID id = idlist.get(ii);

        // check if module and name of this id is already in out[]
        const ScalarResult& d = resultFileMgr->getScalar(id);
        int i;
        int outSize = out.size();
        for (i=0; i<outSize; i++)
        {
            const ScalarResult& vd = resultFileMgr->getScalar(out.get(i));
            if (d.moduleNameRef==vd.moduleNameRef && d.nameRef==vd.nameRef)
                break;
        }

        // not yet -- then add it
        if (i==outSize)
        {
            out.add(id);
            if (outSize>maxcount)
                break; // enough is enough
        }
    }

    return out;
}

IDVectorVector ScalarDataSorter::prepareCopyToClipboard(const IDList& idlist)
{
    // form groups (IDVectors) by fileRef+runNumber+moduleNameRef
    IDVectorVector vv = doGrouping(idlist, sameGroupFileRunModule);

    // order each group by scalar name, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same scalarNameRefs
    sortAndAlign(vv, lessByScalarNameRef, equalByScalarNameRef);

    return vv;
}

