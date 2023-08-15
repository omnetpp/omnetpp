//==========================================================================
//   SCAVE_BINDINGS.CC  -  part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2023 Andras Varga
  Copyright (C) 2006-2023 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <nanobind/nanobind.h>
#include <nanobind/make_iterator.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/map.h>
#include <scave/resultfilemanager.h>
#include <scave/interruptedflag.h>
#include <scave/vectorutils.h>

namespace nb = nanobind;

using namespace omnetpp::scave;

#define CONCAT_(prefix, suffix) prefix##suffix
#define CONCAT(prefix, suffix) CONCAT_(prefix, suffix)

#ifndef OMNETPP_MODE_SUFFIX
#define OMNETPP_MODE_SUFFIX
#endif

#define MODULENAME CONCAT(scave_bindings, OMNETPP_MODE_SUFFIX)

NB_MODULE(MODULENAME, m) {

    nb::class_<InterruptedFlag>(m, "InterruptedFlag")
        .def(nb::init())
        .def_rw("flag", &InterruptedFlag::flag)
        ;


    nb::class_<IDList>(m, "IDList")
        .def(nb::init())
        .def(nb::init<ID>())

        .def("isEmpty", &IDList::isEmpty)
        .def("size", &IDList::size)
        .def("get", &IDList::get)
        .def("indexOf", &IDList::indexOf)

        .def("asVector", &IDList::asVector)
        .def("__iter__",
        [](const IDList &v) {
            return nb::make_iterator(nb::type<IDList>(), "iterator",
                                     v.begin(), v.end());
        }, nb::keep_alive<0, 1>());


    nb::class_<ResultFileManager>(m, "ResultFileManager")
        .def(nb::init())

        .def("loadFile", &ResultFileManager::loadFile, nb::rv_policy::reference,
            nb::call_guard<nb::gil_scoped_release>(),
            nb::arg(), nb::arg(), nb::arg(), nb::arg("interrupted").none() = nullptr)

        .def("getSerial", &ResultFileManager::getSerial)
        .def("clear", &ResultFileManager::clear)

        .def("getRuns", &ResultFileManager::getRuns, nb::rv_policy::reference)
        .def("filterRunList", nb::overload_cast<const RunList&, const char *>(&ResultFileManager::filterRunList, nb::const_), nb::rv_policy::reference)

        .def("getAllItems", &ResultFileManager::getAllItems)
        .def("getAllParameters", &ResultFileManager::getAllParameters)
        .def("getAllScalars", &ResultFileManager::getAllScalars, nb::arg() = false)
        .def("getAllVectors", &ResultFileManager::getAllVectors)
        .def("getAllStatistics", &ResultFileManager::getAllStatistics)
        .def("getAllHistograms", &ResultFileManager::getAllHistograms)

        .def("filterIDList", nb::overload_cast<const IDList&, const char *, int, InterruptedFlag *>(&ResultFileManager::filterIDList, nb::const_),
            nb::call_guard<nb::gil_scoped_release>(),
            nb::arg(), nb::arg(), nb::arg() = -1, nb::arg("interrupted").none() = nullptr)

        .def("getNonfieldItem", &ResultFileManager::getNonfieldItem, nb::rv_policy::reference)
        .def("getItem", &ResultFileManager::getItem, nb::rv_policy::reference)
        .def("getFieldScalar", &ResultFileManager::getFieldScalar)
        .def("getNonfieldScalar", &ResultFileManager::getNonfieldScalar, nb::rv_policy::reference)
        .def("getScalar", &ResultFileManager::getScalar, nb::rv_policy::reference)
        .def("getParameter", &ResultFileManager::getParameter, nb::rv_policy::reference)
        .def("getVector", &ResultFileManager::getVector, nb::rv_policy::reference)
        .def("getStatistics", &ResultFileManager::getStatistics, nb::rv_policy::reference)
        .def("getHistogram", &ResultFileManager::getHistogram, nb::rv_policy::reference)
        ;

    using ItemType = decltype(ResultFileManager::PARAMETER);
    nb::enum_<ItemType>(m, "ItemType", nb::is_arithmetic())
        .value("PARAMETER", ResultFileManager::PARAMETER)
        .value("SCALAR", ResultFileManager::SCALAR)
        .value("STATISTICS", ResultFileManager::STATISTICS)
        .value("HISTOGRAM", ResultFileManager::HISTOGRAM)
        .value("VECTOR", ResultFileManager::VECTOR)
        .value("ALL", static_cast<ItemType>( // bonus track
            ResultFileManager::PARAMETER | ResultFileManager::SCALAR | ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM | ResultFileManager::VECTOR)
        )
        ;

    using HostType = ResultFileManager::HostType;
    nb::enum_<HostType>(m, "HostType", nb::is_arithmetic())
        .value("STATISTICS", HostType::HOSTTYPE_STATISTICS)
        .value("HISTOGRAM", HostType::HOSTTYPE_HISTOGRAM)
        .value("VECTOR", HostType::HOSTTYPE_VECTOR)
        ;

    nb::enum_<ResultFileManager::LoadFlags>(m, "LoadFlags", nb::is_arithmetic())
        .value("RELOAD", ResultFileManager::LoadFlags::RELOAD)
        .value("RELOAD_IF_CHANGED", ResultFileManager::LoadFlags::RELOAD_IF_CHANGED)
        .value("NEVER_RELOAD", ResultFileManager::LoadFlags::NEVER_RELOAD)

        .value("ALLOW_INDEXING", ResultFileManager::LoadFlags::ALLOW_INDEXING)
        .value("SKIP_IF_NO_INDEX", ResultFileManager::LoadFlags::SKIP_IF_NO_INDEX)
        .value("ALLOW_LOADING_WITHOUT_INDEX", ResultFileManager::LoadFlags::ALLOW_LOADING_WITHOUT_INDEX)

        .value("SKIP_IF_LOCKED", ResultFileManager::LoadFlags::SKIP_IF_LOCKED)
        .value("IGNORE_LOCK_FILE", ResultFileManager::LoadFlags::IGNORE_LOCK_FILE)

        .value("VERBOSE", ResultFileManager::LoadFlags::VERBOSE)
        .value("LOADFLAGS_DEFAULTS", ResultFileManager::LoadFlags::LOADFLAGS_DEFAULTS)
        ;


    nb::class_<ResultFile>(m, "ResultFile");

    nb::class_<Run>(m, "Run")
        .def("getRunName", &Run::getRunName)

        .def("getAttributes", &Run::getAttributes)
        .def("getAttribute", &Run::getAttribute)

        .def("getIterationVariables", &Run::getIterationVariables)
        .def("getIterationVariable", &Run::getIterationVariable)

        .def("getConfigEntries", &Run::getConfigEntries)
        .def("getParamAssignmentConfigEntries", &Run::getParamAssignmentConfigEntries)
        .def("getNonParamAssignmentConfigEntries", &Run::getNonParamAssignmentConfigEntries)
        ;

    using DataType = ResultItem::DataType;
    nb::enum_<DataType>(m, "DataType", nb::is_arithmetic())
        .value("NA", DataType::TYPE_NA)
        .value("INT", DataType::TYPE_INT)
        .value("DOUBLE", DataType::TYPE_DOUBLE)
        .value("ENUM", DataType::TYPE_ENUM)
        ;

    using FieldNum = ResultItem::FieldNum;
    nb::enum_<FieldNum>(m, "FieldNum", nb::is_arithmetic())
        .value("NONE", FieldNum::NONE)
        .value("COUNT", FieldNum::COUNT)
        .value("SUM", FieldNum::SUM)
        .value("SUMWEIGHTS", FieldNum::SUMWEIGHTS)
        .value("MEAN", FieldNum::MEAN)
        .value("STDDEV", FieldNum::STDDEV)
        .value("MIN", FieldNum::MIN)
        .value("MAX", FieldNum::MAX)
        .value("NUMBINS", FieldNum::NUMBINS)
        .value("RANGEMIN", FieldNum::RANGEMIN)
        .value("RANGEMAX", FieldNum::RANGEMAX)
        .value("UNDERFLOWS", FieldNum::UNDERFLOWS)
        .value("OVERFLOWS", FieldNum::OVERFLOWS)
        .value("STARTTIME", FieldNum::STARTTIME)
        .value("ENDTIME", FieldNum::ENDTIME)
        ;

    nb::class_<ResultItem>(m, "ResultItem")
        .def("getRun", &ResultItem::getRun, nb::rv_policy::reference)
        .def("getName", &ResultItem::getName)
        .def("getModuleName", &ResultItem::getModuleName)

        .def("getItemType", &ResultItem::getItemType)
        .def("getItemTypeString", &ResultItem::getItemTypeString)
        .def_static("itemTypeToString", &ResultItem::itemTypeToString)

        .def("getAttribute", &ResultItem::getAttribute)
        .def("getAttributes", &ResultItem::getAttributes)
        ;

    nb::class_<ScalarResult, ResultItem>(m, "ScalarResult")
        .def(nb::init<>())
        .def("getValue", &ScalarResult::getValue)
        .def("isField", &ScalarResult::isField)
        ;

    nb::class_<VectorResult, ResultItem>(m, "VectorResult")
        .def("getStatistics", &VectorResult::getStatistics)
        ;

    nb::class_<StatisticsResult, ResultItem>(m, "StatisticsResult")
        .def("getStatistics", &StatisticsResult::getStatistics)
        ;

    nb::class_<HistogramResult, StatisticsResult>(m, "HistogramResult")
        .def("getHistogram", &HistogramResult::getHistogram)
        ;

    nb::class_<ParameterResult, ResultItem>(m, "ParameterResult")
        .def("getValue", &ParameterResult::getValue)
        ;

    nb::class_<Statistics>(m, "Statistics")
        .def("isWeighted", &Statistics::isWeighted)
        .def("getMin", &Statistics::getMin)
        .def("getMax", &Statistics::getMax)
        .def("getCount", &Statistics::getCount)
        .def("getSumWeights", &Statistics::getSumWeights)
        .def("getMean", &Statistics::getMean)
        .def("getStddev", &Statistics::getStddev)
        .def("getVariance", &Statistics::getVariance)
        .def("getSum", &Statistics::getSum)
        .def("getSumSqr", &Statistics::getSumSqr)
        .def("getWeightedSum", &Statistics::getWeightedSum)
        .def("getSumSquaredWeights", &Statistics::getSumSquaredWeights)
        .def("getSumWeightedSquaredValues", &Statistics::getSumWeightedSquaredValues)
        ;

    nb::class_<Histogram>(m, "Histogram")
        .def("getBinEdges", &Histogram::getBinEdges)
        .def("getBinValues", &Histogram::getBinValues)
        .def("getNumBins", &Histogram::getNumBins)
        .def("getBinEdge", &Histogram::getBinEdge)
        .def("getBinValue", &Histogram::getBinValue)
        .def("getUnderflows", &Histogram::getUnderflows)
        .def("getOverflows", &Histogram::getOverflows)
        ;

    m.def("readVectorsIntoArrays", &readVectorsIntoArrays,
        nb::arg(), nb::arg(),
        nb::arg("includePreciseX"), nb::arg("includeEventNumbers"),
        nb::arg("memoryLimitBytes") = std::numeric_limits<size_t>::max(),
        nb::arg("simTimeStart") = -INFINITY, nb::arg("simTimeEnd") = INFINITY,
        nb::arg("interrupted").none() = nullptr)
        ;

    nb::class_<XYArray>(m, "XYArray")
        .def("length", &XYArray::length)
        .def("getX", &XYArray::getX)
        .def("getY", &XYArray::getY)
        //.def("getPreciseX", &XYArray::getPreciseX)
        .def("getEventNumber", &XYArray::getEventNumber)
        ;
}
