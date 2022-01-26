"""
Contains operations that can be applied to vectors.

In the IDE, operations can be applied to vectors on a vector chart by means
of the plot's context menu and by editing the *Vector Operations* field in
the chart configuration dialog.

Every vector operation is implemented as a function. The notation used
in the documentation of the individual functions is:

- *y[k]*: The kth value in the input
- *t[k]*: The kth timestamp in the input
- *yout[k]*: The kth value in the output
- *tout[k]*: The kth timestamp in the output

A vector operation function accepts a DataFrame row as the first positional
argument, and optionally additional arguments specific to its operation.
When the function is invoked, the row will contain a `vectime` and a `vecvalue`
column (both containing NumPy `ndarray`'s) that are the input of the operation.
The function should return a similar row, with updated `vectime` and a `vecvalue`
columns.

Additionally, the operation may update the `name` and `title` columns (provided
they exist) to reflect the processing in the name. For example, an operation
that computes *mean* may return `mean(%s)` as name and `Mean of %s` as title
(where `%s` indicates the original name/title).

The `aggregate()` and `merge()` functions are special. They receive a DataFrame
instead of a row in the first argument, and return new DataFrame with the result.

Vector operations can be applied to a DataFrame using `utils.perform_vector_ops(df,ops)`.
`ops` is a multiline string where each line denotes an operation; they are
applied in sequence. The syntax of one operation is:

[(`compute`|`apply`) `:` ] *opname* [ `(` *arglist* `)` ] [ `#` *comment* ]

*opname* is the name of the function, optionally qualified with its package name.
If the package name is omitted, `omnetpp.scave.vectorops` is assumed.

`compute` and `apply` specify whether the newly computed vectors will replace
the input row in the DataFrame (*apply*) or added as extra lines (*compute*).
The default is *apply*.


To register a new vector operation, define a function that fulfills the above interface
(e.g. in the chart script, or an external `.py` file, that the chart script imports),
with the `omnetpp.scave.vectorops.vector_operation` decorator on it.

Make sure that the registered function does not modify the data of the NumPy array
instances in the rows, because it would have an unwanted effect when used in `compute`
(as opposed to `apply`) mode.

Example:

```
from omnetpp.scave import vectorops

@vectorops.vector_operation("Fooize", "foo(42)")
def foo(r, arg1, arg2=5):
    # r.vectime = r.vectime * 2    # <- this is okay
    # r.vectime *= 2               # <- this is NOT okay!

    r.vectime = r.vectime * arg1 + arg2
    if "title" in r:
        r.title = r.title + ", but fooized" # this is also okay
    return r
```
"""

import numpy as np
import pandas as pd
import inspect

from ._version import __version__

def perform_vector_ops(df, operations : str):
    """ See: utils.perform_vector_ops """
    import omnetpp.scave.utils as utils
    return utils.perform_vector_ops(df, operations)


"""
An internal registry of the available vector operations.
Stores 3-tuples of: (<function>, "label", "example")
The <function> is the Python function itself, the "label"
is what will appear on the GUI, and "example" is what will be
inserted intially when the operation is chosen by the user.
"""
_operations = []


def _get_vectorop_signature(func):
    """
    Returns the signature of `func` as a string that can be used to prescribe it
    as a vector operation - so, without the first parameter, which is used
    internally to pass the DataFrame/row to process.
    """
    import functools
    m = functools.partial(func, None) # binding the row parameter
    return str(inspect.signature(m))


def vector_operation(label : str = None, example : str = None):
    """
    Returns, or acts as, a decorator; to be used on methods you wish to register as vector operations.
    Parameters:
      - `label`: will be shown on the GUI for the user
      - `example`: should be string, containing a valid invocation of the function
    Alternatively, this can also be used directly as decorator (without calling it first).
    """

    def decorator(function):
        global _operations
        lbl = label or function.__name__.title()
        ex = example or (function.__name__ + _get_vectorop_signature(function))

        if not isinstance(lbl, str):
            raise ValueError("@vector_operation: label argument must be string")
        if not isinstance(ex, str):
            raise ValueError("@vector_operation: example argument must be string")

        _operations.append((function, lbl, ex))
        return function

    if inspect.isfunction(label):
        # In case the user omitted the () after @vector_operation, Python will call _this_ method
        # directly, with the vector operation function itself in place of the label parameter.
        # We could error out, OR we could cheat and recover, like this:
        function = label
        label = None
        decorator(function)
        return function
    else:
        return decorator


def lookup_operation(module, name):
    """
    Returns a function from the registered vector operations by name, and optionally module.
    `module` and `name` are both strings. `module` can also be `None`, in which case it is ignored.
    """
    global _operations
    for fn, _label, _example in _operations:
        if (module is None or fn.__module__ == module) and fn.__name__ == name:
            return fn
    return None


@vector_operation
def aggregate(df, function='average'):
    """
    Aggregates several vectors into a single one, aggregating the
    y values *at the same time coordinate* with the specified function.
    Possible values: 'sum', 'average', 'count', 'maximum', 'minimum'
    """
    vectimes = df['vectime']
    vecvalues = df['vecvalue']

    # the number of rows in the DataFrame
    n = len(df.index)
    indices = [0] * n

    # the sum of all vector lengths
    capacity = vectimes.apply(len).sum()

    # these are uninitialized, and might be oversized, but always large enough
    out_times = np.empty(capacity)
    out_values = np.empty(capacity)
    out_index = 0

    while True:
        current_time = 0
        times = []
        for i in range(n):
            if len(vectimes[i]) > indices[i]:
                times.append(vectimes[i][indices[i]])

        if times:
            current_time = np.min(times)
        else:
            break

        values_now = []

        for i in range(n):
            while len(vectimes[i]) > indices[i]:
                time = vectimes[i][indices[i]]
                value = vecvalues[i][indices[i]]

                if time == current_time:
                    values_now.append(value)
                    indices[i] += 1
                else:
                    break

        if values_now:
            outval = 0

            if function == 'sum':
                outval = np.sum(values_now)
            elif function == 'average':
                outval = np.average(values_now)
            elif function == 'count':
                outval = len(values_now)
            elif function == 'maximum':
                outval = np.max(values_now)
            elif function == 'minimum':
                outval = np.min(values_now)
            else:
                raise Exception("unknown aggregation function")

            out_times[out_index] = current_time
            out_values[out_index] = outval

            out_index += 1
            # ASSERT out_index < len(out_values)

    out_times = np.resize(out_times, out_index)
    out_values = np.resize(out_values, out_index)

    comment = "aggregate({}) of {} vectors".format(function, n)
    out_df = _combine_rows(df, out_times, out_values, comment)
    return out_df


@vector_operation
def merge(df):
    """
    Merges several series into a single one, maintaining increasing
    time order in the output.
    """
    vectimes = df['vectime']
    vecvalues = df['vecvalue']

    # the number of rows in the DataFrame
    n = len(df.index)
    indices = [0] * n

    # the sum of all vector lengths
    capacity = vectimes.apply(len).sum()
    # these are uninitialized
    out_times = np.empty(capacity)
    out_values = np.empty(capacity)
    out_index = 0

    while True:
        current_time = 0
        times = []
        for i in range(n):
            if len(vectimes[i]) > indices[i]:
                times.append(vectimes[i][indices[i]])

        if times:
            current_time = np.min(times)
        else:
            break

        values_now = []

        for i in range(n):
            while len(vectimes[i]) > indices[i]:
                time = vectimes[i][indices[i]]
                value = vecvalues[i][indices[i]]

                if time == current_time:
                    values_now.append(value)
                    indices[i] += 1
                else:
                    break

        for v in values_now:
            out_times[out_index] = current_time
            out_values[out_index] = v

            out_index += 1
            # ASSERT out_index < len(out_values)

    out_times = np.resize(out_times, out_index)
    out_values = np.resize(out_values, out_index)

    comment = "merged from {} vectors".format(n)
    out_df = _combine_rows(df, out_times, out_values, comment)
    return out_df

def _combine_rows(df, out_times, out_values, comment):
    def column_brief(df, col):
        if not col in df:
            return None
        names = df[col].unique()
        n = len(names)
        if n == 0:
            return "n/a"
        elif n == 1:
            return names[0]
        else:
            for i in ["", np.nan, None]: # move empty values to the back so that aren't chosen as example if at all possible
                if i in names:
                    names.remove(i); names.append(i)  # move to back
            return str(names[0]) + " and " + str(n-1) + " more"

    data = { 'vectime': [ out_times ], 'vecvalue': [ out_values ], 'comment': [ comment ] }
    for column in df:
        if column not in data:
            data[column] = [ column_brief(df, column) ]

    result = pd.DataFrame(data)
    return result

@vector_operation
def mean(r):
    """
    Computes mean on (0,t): yout[k] = sum(y[i], i=0..k) / (k+1).
    """
    v = r['vecvalue']
    r['vecvalue'] = np.cumsum(v) / np.arange(1, len(v) + 1)
    if "title" in r:
        r['title'] = 'Mean of ' + r['title']
    return r


@vector_operation
def sum(r):
    """
    Sums up values: yout[k] = sum(y[i], i=0..k)
    """
    r['vecvalue'] = np.cumsum(r['vecvalue'])
    if "title" in r:
        r['title'] = 'Cumulative sum of ' + r['title']
    return r


@vector_operation("Add constant", "add(100)")
def add(r, c):
    """
    Adds a constant to all values in the input: yout[k] = y[k] + c
    """
    v = r['vecvalue']
    r['vecvalue'] = v + c
    if "title" in r:
        r['title'] = r['title'] + " + " + str(c)
    return r


@vector_operation("Compare with threshold", "compare(threshold=9000, less=-1, equal=0, greater=1)")
def compare(r, threshold, less=None, equal=None, greater=None):
    """
    Compares value against a threshold, and optionally replaces it with a constant.
    yout[k] = if y[k] < threshold and less != None then less;
         else if y[k] == threshold and equal != None then equal;
         else if y[k] > threshold and greater != None then greater;
         else y[k]
    The last three parameters are all independently optional.
    """
    v = r['vecvalue']

    if less is not None:
        less_mask = v < threshold
        v = np.where(less_mask, less, v)

    if equal is not None:
        equal_mask = v == threshold
        v = np.where(equal_mask, equal, v)

    if greater is not None:
        greater_mask = v > threshold
        v = np.where(greater_mask, greater, v)

    r['vecvalue'] = v

    if "title" in r:
        r['title'] = r['title'] + " compared to " + str(threshold)

    return r


@vector_operation("Crop in time", "crop(t1=10, t2=100)")
def crop(r, t1, t2):
    """
    Discards values outside the [t1, t2] interval.
    The time values are in seconds.
    """
    t = r['vectime']
    v = r['vecvalue']

    from_index = np.searchsorted(t, t1, 'left')
    to_index = np.searchsorted(t, t2, 'right')

    r['vectime'] = t[from_index:to_index]
    r['vecvalue'] = v[from_index:to_index]

    if "title" in r:
        r['title'] = r['title'] + " on [{}s,{}s]".format(t1, t2)
    return r


@vector_operation
def difference(r):
    """
    Subtracts the previous value from every value: yout[k] = y[k] - y[k-1]
    """
    v = r['vecvalue']

    r['vecvalue'] = v - np.concatenate([np.array([0]), v[:-1]])

    if "title" in r:
        r['title'] = "Difference of " + r['title']
    return r


@vector_operation("Difference quotient")
def diffquot(r):
    """
    Calculates the difference quotient of every value and the subsequent one:
    yout[k] = (y[k+1]-y[k]) / (t[k+1]-t[k])
    """
    t = r['vectime']
    v = r['vecvalue']

    dt = t[1:] - t[:-1]
    dv = v[1:] - v[:-1]

    r['vecvalue'] = dv / dt
    r['vectime'] = t[:-1]

    if "title" in r:
        r['title'] = "Difference quotient of " + r['title']
    return r


@vector_operation("Divide by constant", "divide_by(1000)")
def divide_by(r, a):
    """
    Divides every value in the input by a constant: yout[k] = y[k] / a
    """
    v = r['vecvalue']
    r['vecvalue'] = v / a
    if "title" in r:
        r['title'] = r['title'] + " / " + str(a)
    return r


@vector_operation("Divide by time")
def divtime(r):
    """
    Divides every value in the input by the corresponding time: yout[k] = y[k] / t[k]
    """
    t = r['vectime']
    v = r['vecvalue']
    r['vecvalue'] = v / t
    if "title" in r:
        r['title'] = r['title'] + " / t "
    return r


@vector_operation(example="expression('y + (t - tprev) * 100')")
def expression(r, expression, as_time=False):
    """
    Replaces the value with the result of evaluating the Python arithmetic expression
    given as a string: yout[k] = eval(expression). The expression may use
    the following variables: `t`, `y`, `tprev`, `yprev`, `tnext`, `ynext`, `k`, `n`
    which stand for t[k], y[k], t[k-1], y[k-1], t[k+1] and y[k+1], k, and the
    size of vector, respectively.

    If `as_time` is `True`, the result will be assigned to the time variable
    instead of the value variable.

    Note that for efficiency, the expression will be evaluated only once,
    with the variables being `np.ndarray` instances instead of scalar `float` values.
    Thus, the result is computed using vector operations instead of looping
    through all vector indices in Python. Expression syntax remains the usual.
    Most Numpy mathematical functions can be used without module prefix; other
    Numpy functions can be used by prefixing them with `np.`.

    Examples: `2*y+0.5`, `abs(floor(y))`, `(y-yprev)/(t-tprev)`, `fmin(yprev,ynext)`,
    `cumsum(y)`, `nan_to_num(y)`
    """
    t = r['vectime']
    y = r['vecvalue']
    n = len(y)
    k = np.arange(0,n) if "k" in expression else None
    tprev = np.concatenate([np.array([0]), t[:-1]]) if "tprev" in expression else None
    yprev = np.concatenate([np.array([np.nan]), y[:-1]]) if "yprev" in expression else None
    tnext = np.concatenate([t[1:], np.array([np.nan])]) if "tnext" in expression else None
    ynext = np.concatenate([y[1:], np.array([np.nan])]) if "ynext" in expression else None

    # make mathematical functions available in the expression (note: "import *" is not allowed in function scope)
    from numpy import sin,cos,tan,arcsin,arccos,arctan,hypot,arctan2,degrees,radians,unwrap,deg2rad,\
        rad2deg,sinh,cosh,tanh,arcsinh,arccosh,arctanh,around,round_,rint,fix,floor,ceil,\
        trunc,prod,sum,nanprod,nansum,cumprod,cumsum,nancumprod,nancumsum,diff,ediff1d,\
        gradient,cross,trapz,exp,expm1,exp2,log,log10,log2,log1p,logaddexp,logaddexp2,i0,\
        sinc,signbit,copysign,frexp,ldexp,nextafter,spacing,lcm,gcd,add,reciprocal,\
        positive,negative,multiply,divide,power,subtract,true_divide,floor_divide,\
        float_power,fmod,mod,modf,remainder,divmod,angle,real,imag,conj,conjugate,\
        convolve,clip,sqrt,cbrt,square,absolute,fabs,sign,heaviside,maximum,minimum,\
        fmax,fmin,nan_to_num,real_if_close,interp

    result = eval(expression)
    result = result if type(result) == np.ndarray else np.full(n, result) # make expression="5" work, too
    if as_time:
        r['vectime'] = result
    else:
        r['vecvalue'] = result
    if len(result) != n:
        raise RuntimeError("Expression returned an ndarray of different size ({} instead of {})".format(len(result),n))

    if "title" in r:
        r['title'] = r['title'] + ": " + expression
    return r


def _integrate_helper(t, v, interpolation):
    dt = np.concatenate([np.array([0]), t[1:] - t[:-1]])
    vprev = np.concatenate([np.array([0]), v[:-1]])

    if interpolation == 'sample-hold':
        increments = dt * vprev
    elif interpolation == 'backward-sample-hold':
        increments = dt * v
    elif interpolation == 'linear':
        increments = dt * (v + vprev) / 2
    else:
        raise ValueError("unknown interpolation '{}', available ones are: 'linear', 'sample-hold', 'backward-sample-hold'", format(interpolation))

    return np.cumsum(increments)

@vector_operation(example="integrate(interpolation='linear')")
def integrate(r, interpolation='sample-hold'):
    """
    Integrates the input as a step function ("sample-hold" or "backward-sample-hold")
    or with linear ("linear") interpolation.
    """
    t = r['vectime']
    v = r['vecvalue']

    r['vecvalue'] = _integrate_helper(t, v, interpolation)

    if "title" in r:
        r['title'] = r['title'] + " integrated " + interpolation
    return r


@vector_operation("Linear trend", "lineartrend(0.5)")
def lineartrend(r, a):
    """
    Adds a linear component with the given steepness to the input series: yout[k] = y[k] + a * t[k]
    """
    t = r['vectime']
    v = r['vecvalue']

    r['vecvalue'] = v + a * t

    if "title" in r:
        r['title'] = r['title'] + " + " + str(a) + " * t"
    return r


@vector_operation(example="modulo(256.0)")
def modulo(r, m):
    """
    Computes floating point reminder (modulo) of the input values with a constant: yout[k] = y[k] % m
    """
    v = r['vecvalue']
    r['vecvalue'] = np.remainder(v, m)
    if "title" in r:
        r['title'] = r['title'] + " mod " + str(m)
    return r


@vector_operation("Moving average", "movingavg(alpha=0.1)")
def movingavg(r, alpha):
    """
    Applies the exponentially weighted moving average filter with
    the given smoothing coefficient in range (0.0, 1.0]:
    yout[k] = yout[k-1] + alpha * (y[k]-yout[k-1])
    """
    v = r['vecvalue'].copy() # modification in-place is not allowed
    s = pd.Series(v, dtype=np.dtype('f8'))
    r['vecvalue'] = s.ewm(alpha=alpha).mean().values
    if "title" in r:
        r['title'] = r['title'] + " mean " + str(alpha)
    return r


@vector_operation("Multiply by constant", "multiply_by(2)")
def multiply_by(r, a):
    """
    Multiplies every value in the input by a constant: yout[k] = a * y[k]
    """
    v = r['vecvalue']
    r['vecvalue'] = v * a
    if "title" in r:
        r['title'] = r['title'] + " * " + str(a)
    return r


@vector_operation("Remove repeating values")
def removerepeats(r):
    """
    Removes repeated (consecutive) y values
    """
    t = r['vectime']
    v = r['vecvalue']

    is_repeating = v[:-1] == v[1:]
    not_repeating = np.concatenate([[1], 1 - is_repeating])

    r['vecvalue'] = np.compress(not_repeating, v)
    r['vectime'] = np.compress(not_repeating, t)

    if "title" in r:
        r['title'] = r['title'] + " removerepeats"
    return r


@vector_operation("Sliding window average", "slidingwinavg(window_size=10)")
def slidingwinavg(r, window_size, min_samples=None):
    """
    Replaces every value with the mean of values in the window:
    yout[k] = sum(y[i], i=(k-winsize+1)..k) / winsize
    If min_samples is also given, allows each window to have only that many
    valid (not missing [at the ends], and not NaN) samples in each window.
    """
    v = r['vecvalue']
    s = pd.Series(v, dtype=np.dtype('f8'))
    r['vecvalue'] = s.rolling(window_size, min_periods=min_samples).mean().values
    if "title" in r:
        r['title'] = r['title'] + " windowmean " + str(window_size)
    return r


@vector_operation("Subtract first value")
def subtractfirstval(r):
    """
    Subtract the first value from every subsequent value: yout[k] = y[k] - y[0]
    """
    v = r['vecvalue']
    r['vecvalue'] = v - v[0]
    if "title" in r:
        r['title'] = r['title'] + " - v[0]"
    return r


@vector_operation("Time average", "timeavg(interpolation='linear')")
def timeavg(r, interpolation):
    """
    Average over time (integral divided by time), possible
    parameter values: 'sample-hold', 'backward-sample-hold', 'linear'
    """
    # TODO: add "auto" - where we choose interpolation based on interpolationmode and enum
    t = r['vectime']
    v = r['vecvalue']

    integrated = _integrate_helper(t, v, interpolation)

    r['vecvalue'] = integrated / t
    if "title" in r:
        r['title'] = r['title'] + " timeavg"

    r["interpolationmode"] = "linear"

    return r


@vector_operation("Time difference")
def timediff(r):
    """
    Sets each value to the elapsed time (delta) since the previous value:
    tout[k] = t[k] - t[k-1]
    """
    t = r['vectime']

    r['vecvalue'] = np.concatenate([np.array([0]), t[1:] - t[:-1]])

    if "title" in r:
        r['title'] = r['title'] + " timediff"
    return r


@vector_operation("Time shift", "timeshift(dt=100)")
def timeshift(r, dt):
    """
    Shifts the input series in time by a constant (in seconds): tout[k] = t[k] + dt
    """
    t = r['vectime']

    r['vectime'] = t + dt

    if "title" in r:
        r['title'] = r['title'] + " shifted by " + str(dt)
    return r


@vector_operation("Time dilation", "timedilation(c=1)")
def timedilation(r, c):
    """
    Dilates the input series in time by a constant factor: tout[k] = t[k] * c
    """
    t = r['vectime']

    r['vectime'] = t * c

    if "title" in r:
        r['title'] = r['title'] + " dilated by " + str(c)
    return r


@vector_operation("Time to serial")
def timetoserial(r):
    """
    Replaces time values with their index: tout[k] = k
    """
    t = r['vectime']

    r['vectime'] = np.arange(0, len(t))

    if "title" in r:
        r['title'] = r['title'] + " sequence"
    return r


@vector_operation("Time window average", "timewinavg(window_size=0.1)")
def timewinavg(r, window_size=1):
    """
    Calculates time average: Replaces the input values with one every 'window_size'
    interval (in seconds), that is the mean of the original values in that interval.
    tout[k] = k * winSize,
    yout[k] = average of y values in the [(k-1) * winSize, k * winSize) interval
    """
    t = r['vectime']
    v = r['vecvalue']

    t2 = t / window_size
    bucket = np.floor(t2)

    grouped = pd.Series(v, dtype=np.dtype('f8')).groupby(bucket).mean()

    r['vectime'] = grouped.index.values * window_size
    r['vecvalue'] = grouped.values

    if "title" in r:
        r['title'] = r['title'] + " timewinavg"
    return r


@vector_operation("Time window thruput")
def timewinthruput(r, window_size=1):
    """
    Calculates time windowed throughput:
    tout[k] = k * winSize,
    yout[k] = sum of y values in the [(k-1) * winSize, k * winSize) interval divided by window_size
    """
    t = r['vectime']
    v = r['vecvalue']

    tb = np.arange(0, t[-1] + window_size, window_size)

    hist, bins = np.histogram(t, bins=tb, weights=v)

    r['vectime'] = bins[1:]
    r['vecvalue'] = hist / window_size

    if "title" in r:
        r['title'] = r['title'] + " timewinthruput"
    if "unit" in r:
        r['unit'] = r['unit'] + "/s"
    if "interpolationmode" in r:
        r['interpolationmode'] = "linear"
    return r


@vector_operation("Window average", "winavg(window_size=10)")
def winavg(r, window_size=10):
    """
    Calculates batched average: replaces every 'winsize' input values
    with their mean. Time is the time of the first value in the batch.
    """
    window_size = int(window_size)
    t = r['vectime']
    v = r['vecvalue']

    t2 = np.arange(0, len(t)) / window_size
    bucket = np.floor(t2)

    grouped = pd.Series(v, dtype=np.dtype('f8')).groupby(bucket).mean()

    r['vectime'] = t[::window_size]
    r['vecvalue'] = grouped.values

    if "title" in r:
        r['title'] = r['title'] + " winavg"
    return r


def _report_ops():
    """
    Internal. Returns a list of the registered vector operations, each as a tuple of 6 strings:
    ("module", "name", "signature", "docstring", "label", "example")
    """
    import inspect

    result = []
    global _operations
    for op in _operations:
        result.append((
            op[0].__module__,
            str(op[0].__name__),
            _get_vectorop_signature(op[0]),
            inspect.getdoc(op[0]),
            str(op[1]),
            str(op[2])))

    return result