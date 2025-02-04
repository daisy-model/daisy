'''
Example illustrating how state is kept over repeated calls to a python function from Daisy
See the Daisy program in accumulate.dai
'''
import math

class Accumulate():
    def __init__(self, acc=0, method='sum'):
        self.acc = acc
        self.method = method

    def __call__(self, x):
        if self.method == 'logsum':
            self.acc += math.log(x)
        else:
            self.acc += x
        return self.acc
    
accumulate = Accumulate(method='logsum')
# Uncomment the line below to accumulate as a sum, instead of as a sum of logaithms
#accumulate = Accumulate(method='sum')
        
