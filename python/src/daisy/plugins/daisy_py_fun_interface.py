'''Abstract base class for python functions callable from Daisy.
See daisy_py_fun_example.py for a concrete usage  example.
'''
from abc import ABCMeta, abstractmethod

class DaisyPyFun(metaclass=ABCMeta):
    '''A class that can be instantiated and called from Daisy'''

    @abstractmethod
    def __init__(self, args: list):
        '''The constructor can be used to parameterize the function. It must accept a single list as
        parameter.
        '''

    @abstractmethod
    def __call__(self, x: float) -> float:
        '''Apply the function on x.'''

    @property
    @abstractmethod
    def range(self) -> str:
        '''String representation of the units of the range of the function. Note that this should be
        overriden by a property, not by a method.'''

    @property
    @abstractmethod
    def domain(self) -> str:
        '''String representation of the units of the domain of the function. Note that this should be
        overriden by a property, not by a method.'''
