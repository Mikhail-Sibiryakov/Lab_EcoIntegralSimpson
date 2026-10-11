"""Independent comparison; this script is not linked to or called by Eco.

Uses mpmath tanh-sinh at 50 decimal working digits, checked at 70 digits
and against analytic antiderivatives. Decimal results never pass through float.
Reference: https://mpmath.org/doc/current/calculus/integration.html
"""

import argparse
import csv
from functools import lru_cache
from pathlib import Path

import mpmath as mp


DIGITS = {"float": 4, "double": 8, "longdouble": 10}
CASES = {
    "constant", "quadratic", "power14", "peak", "odd", "negative",
    "small", "large", "absolute_only", "relative_only",
}


def integrand(case):
    """Mathematical functions matching the documented test inputs."""
    if case == "constant":
        return lambda x: mp.mpf(1)
    if case == "power14":
        return lambda x: x**14
    if case == "peak":
        return lambda x: 1 / (1 + 400 * (x - mp.mpf(3) / 8)**2)
    if case == "odd":
        return lambda x: x - mp.mpf(1) / 2
    scales = {
        "quadratic": "1", "absolute_only": "1", "relative_only": "1",
        "negative": "-1", "small": "0.0001", "large": "100",
    }
    scale = mp.mpf(scales[case])
    return lambda x: scale * x * x


def analytic(case, a, b):
    """Independent antiderivatives for all examples in the comparison table."""
    if case == "constant":
        return b - a
    if case == "power14":
        return (b**15 - a**15) / 15
    if case == "peak":
        return (mp.atan(20 * (b - mp.mpf(3) / 8))
                - mp.atan(20 * (a - mp.mpf(3) / 8))) / 20
    if case == "odd":
        return ((b - mp.mpf(1) / 2)**2 - (a - mp.mpf(1) / 2)**2) / 2
    scale = integrand(case)(mp.mpf(1))
    return scale * (b**3 - a**3) / 3


@lru_cache(maxsize=None)
def reference(case, a_text, b_text):
    """Check the selected 50-digit reference with two independent checks."""
    values = []
    for precision in (50, 70):
        with mp.workdps(precision):
            a, b = mp.mpf(a_text), mp.mpf(b_text)
            value = mp.quad(integrand(case), [a, b], method="tanh-sinh")
            values.append(value)
    with mp.workdps(70):
        exact = analytic(case, mp.mpf(a_text), mp.mpf(b_text))
        if abs(values[0] - values[1]) > mp.mpf("1e-45"):
            raise ValueError(f"Unstable reference: {case}")
        if abs(values[0] - exact) > mp.mpf("1e-45"):
            raise ValueError(f"Reference disagrees with antiderivative: {case}")
    return values[0]


def compare_row(row):
    """Calculate actual error at 50 digits, without converting strings to float."""
    with mp.workdps(50):
        ref = reference(row["case"], row["a"], row["b"])
        result = mp.mpf(row["result"])
        estimate = mp.mpf(row["estimated_abs_error"])
        epsabs, epsrel = mp.mpf(row["epsabs"]), mp.mpf(row["epsrel"])
        actual = abs(result - ref)
        digits = DIGITS[row["type"]]
        goal = mp.mpf("0.5") * mp.power(10, -digits)
        finite = all(mp.isfinite(v) for v in (result, estimate, epsabs, epsrel))
        criterion = (finite and estimate >= 0 and epsabs >= 0 and epsrel >= 0
                     and (epsabs > 0 or epsrel > 0)
                     and estimate <= max(epsabs, epsrel * abs(result)))
        return {
            **row,
            "N": digits,
            "reference": mp.nstr(ref, 50),
            "actual_abs_error": mp.nstr(actual, 20),
            "goal_met": bool(finite and actual <= goal),
            "criterion_met": bool(criterion),
        }


def read_results(path):
    lines = path.read_text().splitlines()
    if any(line.startswith("FAIL ") for line in lines):
        raise ValueError("UnitTest contains failed checks")
    if not any(line.startswith("SUMMARY PASS ") for line in lines):
        raise ValueError("Successful UnitTest summary is missing")
    fields = ("algorithm", "type", "case", "a", "b", "epsabs", "epsrel",
              "result", "estimated_abs_error", "calls")
    rows = []
    for line in lines:
        if line.startswith("RESULT;"):
            values = line.split(";")[1:]
            if len(values) != len(fields):
                raise ValueError(f"Invalid RESULT record: {line}")
            rows.append(dict(zip(fields, values)))
    expected = {(algorithm, kind, case) for algorithm in ("Simpson",)
                for kind in DIGITS for case in CASES}
    observed = [(r["algorithm"], r["type"], r["case"]) for r in rows]
    if set(observed) != expected or len(observed) != len(expected):
        raise ValueError("Expected exactly 30 distinct Simpson/type/case results")
    return rows


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    lab = Path(__file__).resolve().parents[1]
    parser.add_argument("--log", type=Path,
                        default=lab / "BuildFiles/verification/unit-test.log")
    parser.add_argument("--output", type=Path,
                        default=lab / "BuildFiles/verification")
    args = parser.parse_args()
    rows = [compare_row(row) for row in read_results(args.log)]
    args.output.mkdir(parents=True, exist_ok=True)
    with (args.output / "reference-comparison.csv").open("w", newline="") as output:
        writer = csv.DictWriter(output, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)
    summary = [
        "# Independent reference comparison", "",
        f"mpmath {mp.__version__}, tanh-sinh; dps=50; stability check at dps=70.",
        "All reference values checked against analytic antiderivatives.",
        "Decimal strings are parsed and compared at 50 digits; no Python float conversion.",
        "Goal: absolute error <= 0.5 * 10^(-N) on these examples.", "",
        "| Algorithm | Type | N | Passed | Maximum actual absolute error |",
        "|---|---|---:|---:|---:|",
    ]
    failed = []
    for algorithm in ("Simpson",):
        for kind, digits in DIGITS.items():
            group = [r for r in rows if r["algorithm"] == algorithm and r["type"] == kind]
            passed = sum(r["goal_met"] and r["criterion_met"] for r in group)
            with mp.workdps(50):
                maximum = max(mp.mpf(r["actual_abs_error"]) for r in group)
                summary.append(f"| {algorithm} | {kind} | {digits} | {passed}/{len(group)}"
                               f" | {mp.nstr(maximum, 8)} |")
            failed.extend(r for r in group if not (r["goal_met"] and r["criterion_met"]))
    (args.output / "reference-summary.md").write_text("\n".join(summary) + "\n")
    print(f"Reference comparison: {len(rows) - len(failed)}/{len(rows)} passed")
    for row in failed:
        print("FAIL", row["algorithm"], row["type"], row["case"], row["actual_abs_error"])
    return bool(failed)


if __name__ == "__main__":
    raise SystemExit(main())
