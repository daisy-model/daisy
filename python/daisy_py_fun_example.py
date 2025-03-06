'''Module illustrating how to implement a concrete DaisyPyFun that can be called from Daisy'''
import math
from daisy_py_fun_interface import DaisyPyFun

class Tmin(DaisyPyFun):
    '''A class that calculates the T_min function built into Daisy. See the reference manual for
    details.
    '''
    range = 'None'
    domain = 'dg C'


    def __init__(self, args):
        '''Initialize T_min.

        Parameters
        ----------
        args : list
          If args is not empty, the first parameter is assumed to be a reference value at which the
          function value should be 1.
          If args is empty, the reference value is set to 10.
        '''
        if len(args) == 0:
            self.ref = 10
        else:
            self.ref = float(args[0])


    def __call__(self, t):
        '''Apply Tmin to t. This is the interface exposed to Daisy

        Parameters
        ----------
        t : float
          Temperature in dg C

        Returns
        -------
        float
        '''
        value = self._apply(t)
        if self.ref != 10:
            scale = self._apply(self.ref)
            value = math.nan if scale == 0 else value / scale 
        return value

    def _apply(self, t):
        '''The actual application of Tmin to t. You can have as many properties and methods as you
        like, as long as you implement the interface defined by DaisyPyFun'''
        if t < 0:
            return 0
        if t < 20:
            return 1/self.ref * t
        if t < 37:
            return math.exp(0.47 - 0.027 * t + 0.00193 * t * t)
        if t < 60:
            # J.A. van Veen and M.J.Frissel.
            t_max = 37
            max_val = math.exp(0.47 - 0.027 * t_max + 0.00193 * math.sqrt(t_max))
            return max_val * (1.0 - (t - 37.0) / (60.0 - 37.0))
        return 0
