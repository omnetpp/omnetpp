import numpy as np
import pandas as pd

def perform_vector_ops(df, operations : str):
    import omnetpp.scave.utils as utils
    return utils.perform_vector_ops(df, operations)

def aggregate(df, function='average'):
    """
    Aggregates several vectors into a single one, aggregating the
    y values *at the same time coordinate* with the specified function.
    """
    vectimes = df['vectime']
    vecvalues = df['vecvalue']

    # the number of rows in the DataFrame
    n = len(df.index)
    indices = [0] * n

    # the sum of all vector lengths
    capacity = vectimes.apply(len).sum()
    # print(capacity)
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

def mean(r):
    """
    Computes mean on (0,t): yout[k] = sum(y[i], i=0..k) / (k+1).
    """
    v = r['vecvalue']
    r['vecvalue'] = np.cumsum(v) / np.arange(1, len(v) + 1)
    if "title" in r:
        r['title'] = 'Mean of ' + r['title']
    return r


def sum(r):
    """
    Sums up values: yout[k] = sum(y[i], i=0..k)
    """
    r['vecvalue'] = np.cumsum(r['vecvalue'])
    if "title" in r:
        r['title'] = 'Cumulative sum of ' + r['title']
    return r


def add(r, c):
    """
    Adds a constant to the input: yout[k] = y[k] + c
    """
    v = r['vecvalue']
    r['vecvalue'] = v + c
    if "title" in r:
        r['title'] = r['title'] + " + " + str(c)
    return r


def compare(r, threshold, less=None, equal=None, greater=None):
    """
    Compares value against a threshold, and optionally replaces it with a constant.
    yout[k] = if y[k] < threshold and less != None then less;
         else if y[k] == threshold and equal != None then equal;
         else if y[k] > threshold and greater != None then greater;
         else y[k]
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


def crop(r, t1, t2):
    """
    Discards values outside the [t1, t2] interval
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


def difference(r):
    """
    Subtracts the previous value from every value: yout[k] = y[k] - y[k-1]
    """
    v = r['vecvalue']

    r['vecvalue'] = v - np.concatenate([np.array([0]), v[:-1]])

    if "title" in r:
        r['title'] = "Difference of " + r['title']
    return r


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


def divide_by(r, a):
    """
    Divides input by a constant: yout[k] = y[k] / a
    """
    v = r['vecvalue']
    r['vecvalue'] = v / a
    if "title" in r:
        r['title'] = r['title'] + " / " + str(a)
    return r


def divtime(r):
    """
    Divides input by the current time: yout[k] = y[k] / t[k]
    """
    t = r['vectime']
    v = r['vecvalue']
    r['vecvalue'] = v / t
    if "title" in r:
        r['title'] = r['title'] + " / t "
    return r


def expression(r, expression):
    t = r['vectime']
    y = r['vecvalue']

    tprev = np.concatenate([np.array([0]), t[:-1]])
    yprev = np.concatenate([np.array([0]), y[:-1]])

    r['vecvalue'] = eval(expression)  # TODO - sanitize expression

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
        raise Exception("unknown interpolation: " + interpolation)

    return np.cumsum(increments)


def integrate(r, interpolation='sample-hold'):
    """
    Integrates the input as a step function (sample-hold or backward-sample-hold) or with linear interpolation
    """
    t = r['vectime']
    v = r['vecvalue']

    r['vecvalue'] = _integrate_helper(t, v, interpolation)

    if "title" in r:
        r['title'] = r['title'] + " integrated " + interpolation
    return r


def lineartrend(r, a):
    """
    Adds a linear component to input series: yout[k] = y[k] + a * t[k]
    """
    t = r['vectime']
    v = r['vecvalue']

    r['vecvalue'] = v + a * t

    if "title" in r:
        r['title'] = r['title'] + " + " + str(a) + " * t"
    return r


def modulo(r, m):
    """
    Computes input modulo a constant: yout[k] = y[k] % m
    """
    v = r['vecvalue']
    r['vecvalue'] = np.remainder(v, m)
    if "title" in r:
        r['title'] = r['title'] + " mod " + str(m)
    return r


def movingavg(r, alpha):
    """
    Applies the exponentially weighted moving average filter:
    yout[k] = yout[k-1] + alpha * (y[k]-yout[k-1])
    """
    v = r['vecvalue']
    s = pd.Series(v, dtype=np.dtype('f8'))
    r['vecvalue'] = s.ewm(alpha=alpha).mean().values
    if "title" in r:
        r['title'] = r['title'] + " mean " + str(alpha)
    return r


def multiply_by(r, a):
    """
    Multiplies input by a constant: yout[k] = a * y[k]
    """
    v = r['vecvalue']
    r['vecvalue'] = v * a
    if "title" in r:
        r['title'] = r['title'] + " * " + str(a)
    return r


def removerepeats(r):
    """
    Removes repeated y values
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


def slidingwinavg(r, window_size):
    """
    Replaces every value with the mean of values in the window:
    yout[k] = sum(y[i], i=(k-winsize+1)..k) / winsize
    """
    v = r['vecvalue']
    s = pd.Series(v, dtype=np.dtype('f8'))
    r['vecvalue'] = s.rolling(window_size).mean().values
    if "title" in r:
        r['title'] = r['title'] + " windowmean " + str(window_size)
    return r


def subtractfirstval(r):
    """
    Subtract the first value from every subsequent values: yout[k] = y[k] - y[0]
    """
    v = r['vecvalue']
    r['vecvalue'] = v - v[0]
    if "title" in r:
        r['title'] = r['title'] + " - v[0]"
    return r


def timeavg(r, interpolation):
    """
    Calculates the time average of the input (integral divided by time)
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


def timediff(r):
    """
    Subtracts the previous value's timestamp from every timestamp:
    tout[k] = t[k] - t[k-1]
    """
    t = r['vectime']

    r['vecvalue'] = np.concatenate([np.array([0]), t[1:] - t[:-1]])

    if "title" in r:
        r['title'] = r['title'] + " timediff"
    return r


def timeshift(r, dt):
    """
    Shifts the input series in time by a constant: tout[k] = t[k] + dt
    """
    t = r['vectime']

    r['vectime'] = t + dt

    if "title" in r:
        r['title'] = r['title'] + " shifted by " + str(dt)
    return r


def timetoserial(r):
    """
    Replaces time values with their index: tout[k] = k
    """
    t = r['vectime']

    r['vectime'] = np.arange(0, len(t))

    if "title" in r:
        r['title'] = r['title'] + " sequence"
    return r


def timewinavg(r, window_size=1):
    """
    Calculates time average: replaces input values in every 'windowSize' interval with their mean:
    tout[k] = k * winSize,
    yout[k] = average of y values in the [(k-1)*winSize, k*winSize) interval
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
