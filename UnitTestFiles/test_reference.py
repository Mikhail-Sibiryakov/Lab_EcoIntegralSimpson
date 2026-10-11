"""Behavior checks for independent reference evaluation (outside Eco runtime)."""
import unittest
import mpmath as mp
from compare_reference import compare_row

class ReferenceTests(unittest.TestCase):
    def row(self, case, result, kind='longdouble'):
        return dict(algorithm='Simpson', type=kind, case=case, a='0', b='1',
                    epsabs='2e-13', epsrel='2e-13', result=result,
                    estimated_abs_error='1e-14', calls='5')

    def test_wrong_numeric_result_fails_goal(self):
        self.assertFalse(compare_row(self.row('constant', '2'))['goal_met'])

    def test_decimal_error_survives_without_python_float(self):
        value = compare_row(self.row('quadratic', '0.3333333333333333343333'))
        with mp.workdps(50):
            error = mp.mpf(value['actual_abs_error'])
            self.assertGreater(error, mp.mpf('9e-19'))
            self.assertLess(error, mp.mpf('11e-19'))

    def test_invalid_global_estimate_is_not_accepted(self):
        row = self.row('constant', '1')
        row['estimated_abs_error'] = '-1'
        self.assertFalse(compare_row(row)['criterion_met'])

if __name__ == '__main__':
    unittest.main()
