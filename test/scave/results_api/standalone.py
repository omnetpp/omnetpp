"""
This is all the tests extracted from results_api_test, adapted to the
standalone result loading/processing API.
"""

from omnetpp.scave import results
import pandas as pd
import tester
tester.print = print
from tester import _assert, _assert_sequential_index, run_tests, sanitize_and_compare_csv

RESULT_FILES = ["results/General-*.vec", "results/General-*.sca"]

r = results.read_result_files(RESULT_FILES)

def test_runs():
    df = results.get_runs(r)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "runs.csv"), "content mismatch")

def test_runs_with_itervars():
    df = results.get_runs(r, include_itervars=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "runs_with_itervars.csv"), "content mismatch")

def test_runs_with_runattrs():
    df = results.get_runs(r, include_runattrs=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "runs_with_runattrs.csv"), "content mismatch")

def test_runs_with_param_assignments():
    df = results.get_runs(r, include_param_assignments=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "runs_with_param_assignments.csv"), "content mismatch")

def test_runs_with_config_entries():
    df = results.get_runs(r, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "runs_with_config_entries.csv"), "content mismatch")


def test_runs_with_all():
    df = results.get_runs(r, include_itervars=True, include_runattrs=True, include_param_assignments=True, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "runs_with_all.csv"), "content mismatch")

def test_runs_without_itervars():
    df = results.get_runs(r, include_itervars=False, include_runattrs=True, include_param_assignments=True, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "runs_without_itervars.csv"), "content mismatch")

def test_runs_without_runattrs():
    df = results.get_runs(r, include_itervars=True, include_runattrs=False, include_param_assignments=True, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "runs_without_runattrs.csv"), "content mismatch")

def test_runs_without_param_assignments():
    df = results.get_runs(r, include_itervars=True, include_runattrs=True, include_param_assignments=False, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "runs_without_param_assignments.csv"), "content mismatch")

def test_runs_without_config_entries():
    df = results.get_runs(r, include_itervars=True, include_runattrs=True, include_param_assignments=True, include_config_entries=False)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "runs_without_config_entries.csv"), "content mismatch")


def test_runattrs():
    df = results.get_runattrs(r)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "runattrs.csv"), "content mismatch")

def test_runattrs_with_all():
    df = results.get_runattrs(r, include_itervars=True, include_runattrs=True, include_param_assignments=True, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "runattrs_with_all.csv"), "content mismatch")
    _assert(df.apply(lambda r: r[r["name"]] == r["value"], axis=1).all(), "wrong join")


def test_itervars():
    df = results.get_itervars(r)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "itervars.csv"), "content mismatch")

def test_itervars_with_all():
    df = results.get_itervars(r, include_itervars=True, include_runattrs=True, include_param_assignments=True, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "itervars_with_all.csv"), "content mismatch")
    _assert(df.apply(lambda r: r[r["name"]] == r["value"], axis=1).all(), "wrong join")


def test_config_entries():
    df = results.get_config_entries(r)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "config_entries.csv"), "content mismatch")

def test_config_entries_with_all():
    df = results.get_config_entries(r, include_itervars=True, include_runattrs=True, include_param_assignments=True, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "config_entries_with_all.csv"), "content mismatch")
    _assert(df.apply(lambda r: r[r["name"]] == r["value"], axis=1).all(), "wrong join")


def test_param_assignments():
    df = results.get_param_assignments(r)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "param_assignments.csv"), "content mismatch")

def test_param_assignments_with_all():
    df = results.get_param_assignments(r, include_itervars=True, include_runattrs=True, include_param_assignments=True, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "param_assignments_with_all.csv"), "content mismatch")
    _assert(df.apply(lambda r: r[r["name"]] == r["value"], axis=1).all(), "wrong join")


def test_results():
    df = results.get_results(r)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "results.csv"), "content mismatch")

def test_runattr_count():
    df = results.get_results(r, row_types=["runattr"])
    _assert_sequential_index(df)
    return df["type"].map(lambda t: t == "runattr").all() and df.shape == (96, 4)

def test_itervar_count():
    df = results.get_results(r, row_types=["itervar"])
    _assert_sequential_index(df)
    return df["type"].map(lambda t: t == "itervar").all() and df.shape == (6, 4)

def test_config_count():
    df = results.get_results(r, row_types=["config"])
    _assert_sequential_index(df)
    return  df["type"].map(lambda t: t == "config").all() and df.shape == (18, 4)

def test_unused_columns():
    df = results.get_results(r, row_types=["itervar"], omit_unused_columns=False)
    _assert_sequential_index(df)
    # two replications of three measurements of a single experiment, and 19 columns in total
    return df.shape == (6, 19)


def test_result_filter():
    filtered = results.read_result_files(RESULT_FILES, "type =~ scalar")
    df = results.get_results(filtered)
    _assert_sequential_index(df)
    # in all 6 runs: 20 lines of metadata, and 4 lines (1 scalar and 3 attrs) for all 8 scalars
    return df.shape == (312, 7)

def test_row_type_filter_1():
    df = results.get_results(r, row_types=["scalar"])
    _assert_sequential_index(df)
    # two recorded values from two sources of two submodules in all six runs
    return df.shape == (48, 5)


def test_row_type_filter_2():
    filtered = results.read_result_files(RESULT_FILES, "run =~ *General-0* AND module =~ Test.node1 AND name =~ foo1*")
    df = results.get_results(filtered, row_types=["scalar", "attr"])
    _assert_sequential_index(df)
    # 2 times 4 rows for scalars (incl. value), and 3 times 3 rows for the vector, stats, and histogram (only attr)
    # since we only filtered for row types, not result types, we get the attrs for the other kinds of results too, just not the results themselves
    return df.shape == (17, 7)

def test_row_type_filter_3():
    filtered = results.read_result_files(RESULT_FILES, "type =~ param")
    df = results.get_results(filtered, row_types=["attr"])
    _assert_sequential_index(df)
    # params don't have attrs
    return df.empty

def test_vector_data():
    filtered = results.read_result_files(RESULT_FILES, "type =~ vector AND run =~ General-0*")
    df = results.get_results(filtered, row_types=["vector"])
    _assert_sequential_index(df)
    return df["vectime"].map(lambda a: a.shape == (100,)).all()


def test_vector_time_limit_at_load_1():
    filtered = results.read_result_files(RESULT_FILES, "type =~ vector AND run =~ General-0*", vector_start_time=20.0)
    df = results.get_results(filtered, row_types=["vector"])
    _assert_sequential_index(df)
    return df["vectime"].map(lambda a: (a >= 20.0).all()).all()

def test_vector_time_limit_at_load_2():
    filtered = results.read_result_files(RESULT_FILES, "type =~ vector AND run =~ General-0*", vector_end_time=50.0)
    df = results.get_results(filtered, row_types=["vector"])
    _assert_sequential_index(df)
    return df["vectime"].map(lambda a: (a < 50.0).all()).all()

def test_vector_time_limit_at_load_3():
    filtered = results.read_result_files(RESULT_FILES, "type =~ vector AND run =~ General-0*", vector_start_time=20, vector_end_time=50.0)
    df = results.get_results(filtered, row_types=["vector"])
    _assert_sequential_index(df)
    return df["vectime"].map(lambda a: (a >= 20.0).all() and (a < 50.0).all()).all()



def test_vector_time_limit_1():
    filtered = results.read_result_files(RESULT_FILES, "type =~ vector AND run =~ General-0*")
    df = results.get_results(filtered, row_types=["vector"], start_time=20.0)
    _assert_sequential_index(df)
    return df["vectime"].map(lambda a: (a >= 20.0).all()).all()

def test_vector_time_limit_2():
    filtered = results.read_result_files(RESULT_FILES, "type =~ vector AND run =~ General-0*")
    df = results.get_results(filtered, row_types=["vector"], end_time=50.0)
    _assert_sequential_index(df)
    return df["vectime"].map(lambda a: (a < 50.0).all()).all()

def test_vector_time_limit_3():
    filtered = results.read_result_files(RESULT_FILES, "type =~ vector AND run =~ General-0*")
    df = results.get_results(filtered, row_types=["vector"], start_time=20.0, end_time=50.0)
    _assert_sequential_index(df)
    return df["vectime"].map(lambda a: (a >= 20.0).all() and (a < 50.0).all()).all()


def test_scalars():
    df = results.get_scalars(r)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "scalars.csv"), "content mismatch")

def test_scalars_with_attrs():
    df = results.get_scalars(r, include_attrs=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "scalars_with_attrs.csv"), "content mismatch")

def test_scalars_with_param_assignments():
    df = results.get_scalars(r, include_param_assignments=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "scalars_with_param_assignments.csv"), "content mismatch")

def test_scalars_with_config_entries():
    df = results.get_scalars(r, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "scalars_with_config_entries.csv"), "content mismatch")

def test_scalars_with_all():
    df = results.get_scalars(r, include_attrs=True, include_itervars=True, include_runattrs=True, include_param_assignments=True, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "scalars_with_all.csv"), "content mismatch")


def test_vectors():
    df = results.get_vectors(r)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "vectors.csv"), "content mismatch")


def test_vectors_with_attrs():
    df = results.get_vectors(r, include_attrs=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "vectors_with_attrs.csv"), "content mismatch")


def test_vectors_start_time_at_load():
    trimmed = results.read_result_files(RESULT_FILES, vector_start_time=30)
    df = results.get_vectors(trimmed)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "vectors_start_time.csv"), "content mismatch")

def test_vectors_end_time_at_load():
    trimmed = results.read_result_files(RESULT_FILES, vector_end_time=80)
    df = results.get_vectors(trimmed)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "vectors_end_time.csv"), "content mismatch")

def test_vectors_start_end_time_at_load():
    trimmed = results.read_result_files(RESULT_FILES, vector_start_time=40, vector_end_time=60)
    df = results.get_vectors(trimmed)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "vectors_start_end_time.csv"), "content mismatch")


def test_vectors_start_time():
    trimmed = results.read_result_files(RESULT_FILES)
    df = results.get_vectors(trimmed, start_time=30)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "vectors_start_time.csv"), "content mismatch")

def test_vectors_end_time():
    trimmed = results.read_result_files(RESULT_FILES)
    df = results.get_vectors(trimmed, end_time=80)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "vectors_end_time.csv"), "content mismatch")

def test_vectors_start_end_time():
    trimmed = results.read_result_files(RESULT_FILES)
    df = results.get_vectors(trimmed, start_time=40, end_time=60)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "vectors_start_end_time.csv"), "content mismatch")


def test_histograms():
    df = results.get_histograms(r)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "histograms.csv"), "content mismatch")

def test_histograms_with_attrs():
    df = results.get_histograms(r, include_attrs=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "histograms_with_attrs.csv"), "content mismatch")

def test_histograms_with_all():
    df = results.get_histograms(r, include_attrs=True, include_itervars=True, include_runattrs=True, include_param_assignments=True, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "histograms_with_all.csv"), "content mismatch")


def test_statistics():
    df = results.get_statistics(r)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "statistics.csv"), "content mismatch")

def test_statistics_with_attrs():
    df = results.get_statistics(r, include_attrs=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "statistics_with_attrs.csv"), "content mismatch")

def test_statistics_with_all():
    df = results.get_statistics(r, include_attrs=True, include_itervars=True, include_runattrs=True, include_param_assignments=True, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "statistics_with_all.csv"), "content mismatch")


def test_parameters():
    df = results.get_parameters(r)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "parameters.csv"), "content mismatch")

def test_parameters_with_attrs():
    df = results.get_parameters(r, include_attrs=True)
    _assert_sequential_index(df)
    # these parameters don't have any attrs
    _assert(sanitize_and_compare_csv(df, "parameters.csv"), "content mismatch")

def test_parameters_with_all():
    df = results.get_parameters(r, include_attrs=True, include_itervars=True, include_runattrs=True, include_param_assignments=True, include_config_entries=True)
    _assert_sequential_index(df)
    _assert(sanitize_and_compare_csv(df, "parameters_with_all.csv"), "content mismatch")


run_tests(locals())
