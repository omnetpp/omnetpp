# NOTE: generated automatically as
#
# $ python3 src/scave/python/3rdparty/nanobind/src/stubgen.py \
#     -m omnetpp.scave.scave_bindings -o python/omnetpp/scave/scave_bindings.pyi.dist
#
# in the repo root, after `make MODE=release`.

from collections.abc import Iterator, Sequence
import enum
from typing import Annotated, overload

from numpy.typing import ArrayLike


class DataType(enum.IntEnum):
    NA = 0

    INT = 2

    DOUBLE = 3

    ENUM = 4

class FieldNum(enum.IntEnum):
    NONE = 0

    COUNT = 1

    SUM = 2

    SUMWEIGHTS = 3

    MEAN = 4

    STDDEV = 5

    MIN = 6

    MAX = 7

    NUMBINS = 8

    RANGEMIN = 9

    RANGEMAX = 10

    UNDERFLOWS = 11

    OVERFLOWS = 12

    STARTTIME = 13

    ENDTIME = 14

class Histogram:
    def getBinEdges(self) -> list[float]: ...

    def getBinValues(self) -> list[float]: ...

    def getNumBins(self) -> int: ...

    def getBinEdge(self, arg: int, /) -> float: ...

    def getBinValue(self, arg: int, /) -> float: ...

    def getUnderflows(self) -> float: ...

    def getOverflows(self) -> float: ...

class HistogramResult(StatisticsResult):
    def getHistogram(self) -> Histogram: ...

class HostType(enum.IntEnum):
    STATISTICS = 1

    HISTOGRAM = 2

    VECTOR = 3

class IDList:
    @overload
    def __init__(self) -> None: ...

    @overload
    def __init__(self, arg: int, /) -> None: ...

    def isEmpty(self) -> bool: ...

    def size(self) -> int: ...

    def get(self, arg: int, /) -> int: ...

    def indexOf(self, arg: int, /) -> int: ...

    def asVector(self) -> list[int]: ...

    def __iter__(self) -> Iterator[int]: ...

class InterruptedFlag:
    def __init__(self) -> None: ...

    @property
    def flag(self) -> bool: ...

    @flag.setter
    def flag(self, arg: bool, /) -> None: ...

class ItemType(enum.IntEnum):
    PARAMETER = 1

    SCALAR = 2

    STATISTICS = 4

    HISTOGRAM = 8

    VECTOR = 16

    ALL = 31

class LoadFlags(enum.IntEnum):
    RELOAD = 1

    RELOAD_IF_CHANGED = 2

    NEVER_RELOAD = 4

    ALLOW_INDEXING = 8

    SKIP_IF_NO_INDEX = 16

    ALLOW_LOADING_WITHOUT_INDEX = 32

    SKIP_IF_LOCKED = 64

    IGNORE_LOCK_FILE = 128

    VERBOSE = 256

    LOADFLAGS_DEFAULTS = 74

class ParameterResult(ResultItem):
    def getValue(self) -> str: ...

class ResultFile:
    pass

class ResultFileManager:
    def __init__(self) -> None: ...

    def loadFile(self, arg0: str, arg1: str, arg2: int, interrupted: InterruptedFlag | None = None) -> ResultFile: ...

    def getSerial(self) -> int: ...

    def clear(self) -> None: ...

    def getRuns(self) -> list[Run]: ...

    def filterRunList(self, arg0: Sequence[Run], arg1: str, /) -> list[Run]: ...

    def getAllItems(self, arg: bool, /) -> IDList: ...

    def getAllParameters(self) -> IDList: ...

    def getAllScalars(self, arg: bool = False) -> IDList: ...

    def getAllVectors(self) -> IDList: ...

    def getAllStatistics(self) -> IDList: ...

    def getAllHistograms(self) -> IDList: ...

    def filterIDList(self, arg0: IDList, arg1: str, arg2: int = -1, interrupted: InterruptedFlag | None = None) -> IDList: ...

    def getNonfieldItem(self, arg: int, /) -> ResultItem: ...

    def getItem(self, arg0: int, arg1: ScalarResult, /) -> ResultItem: ...

    def getFieldScalar(self, arg: int, /) -> ScalarResult: ...

    def getNonfieldScalar(self, arg: int, /) -> ScalarResult: ...

    def getScalar(self, arg0: int, arg1: ScalarResult, /) -> ScalarResult: ...

    def getParameter(self, arg: int, /) -> ParameterResult: ...

    def getVector(self, arg: int, /) -> VectorResult: ...

    def getStatistics(self, arg: int, /) -> StatisticsResult: ...

    def getHistogram(self, arg: int, /) -> HistogramResult: ...

class ResultItem:
    def getRun(self) -> Run: ...

    def getName(self) -> str: ...

    def getModuleName(self) -> str: ...

    def getItemType(self) -> int: ...

    def getItemTypeString(self) -> str: ...

    @staticmethod
    def itemTypeToString(arg: int, /) -> str: ...

    def getAttribute(self, arg: str, /) -> str: ...

    def getAttributes(self) -> dict[str, str]: ...

class Run:
    def getRunName(self) -> str: ...

    def getAttributes(self) -> dict[str, str]: ...

    def getAttribute(self, arg: str, /) -> str: ...

    def getIterationVariables(self) -> dict[str, str]: ...

    def getIterationVariable(self, arg: str, /) -> str: ...

    def getConfigEntries(self) -> list[tuple[str, str]]: ...

    def getParamAssignmentConfigEntries(self) -> list[tuple[str, str]]: ...

    def getNonParamAssignmentConfigEntries(self) -> list[tuple[str, str]]: ...

class ScalarResult(ResultItem):
    def __init__(self) -> None: ...

    def getValue(self) -> float: ...

    def isField(self) -> bool: ...

class Statistics:
    def isWeighted(self) -> bool: ...

    def getMin(self) -> float: ...

    def getMax(self) -> float: ...

    def getCount(self) -> int: ...

    def getSumWeights(self) -> float: ...

    def getMean(self) -> float: ...

    def getStddev(self) -> float: ...

    def getVariance(self) -> float: ...

    def getSum(self) -> float: ...

    def getSumSqr(self) -> float: ...

    def getWeightedSum(self) -> float: ...

    def getSumSquaredWeights(self) -> float: ...

    def getSumWeightedSquaredValues(self) -> float: ...

class StatisticsResult(ResultItem):
    def getStatistics(self) -> Statistics: ...

class UnitConversion:
    @staticmethod
    def getBaseUnit(arg: str, /) -> str: ...

    @staticmethod
    def convertUnit(arg0: float, arg1: str, arg2: str, /) -> float: ...

    @staticmethod
    def convertUnitArray(arg0: Annotated[ArrayLike, dict(dtype='float64', shape=(None), order='C', device='cpu')], arg1: str, arg2: str, /) -> None: ...

class VectorResult(ResultItem):
    def getStatistics(self) -> Statistics: ...

class XYArray:
    def length(self) -> int: ...

    def getX(self, arg: int, /) -> float: ...

    def getY(self, arg: int, /) -> float: ...

    def getEventNumber(self, arg: int, /) -> int: ...

def readVectorsIntoArrays(arg0: ResultFileManager, arg1: IDList, includePreciseX: bool, includeEventNumbers: bool, memoryLimitBytes: int = 18446744073709551615, simTimeStart: float = float('-inf'), simTimeEnd: float = float('inf'), interrupted: InterruptedFlag | None = None) -> list[XYArray]: ...

def xyArrayToNumpyArrays(arg0: XYArray, arg1: Annotated[ArrayLike, dict(dtype='float64', shape=(None), order='C', device='cpu')], arg2: Annotated[ArrayLike, dict(dtype='float64', shape=(None), order='C', device='cpu')], /) -> None: ...
