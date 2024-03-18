'''Module for reading Daisy log files'''
import re
from collections import namedtuple
import pandas as pd

__all__ = [
    'Dlf',
    'read_dlf'
]

Dlf = namedtuple('Dlf', ['header', 'units', 'body'])
Dlf.__doc__ += ': Daisy model output log'
Dlf.header.__doc__ = 'dict with information describing log'
Dlf.units.__doc__ = 'dict mapping value names to units'
Dlf.body.__doc__ = 'pandas.DataFrame containing the logged values'

def read_dlf(path):
    '''Read a daisy log file

    Parameters
    ----------
    path : str
      Path to daisy log file

    Returns
    -------
    Dlf object
    '''
    header_body_sep = '--------------------'
    with open(path, encoding='utf-8') as infile:
        header = {}
        for row in infile:
            if row.startswith(header_body_sep):
                break
            row = row.strip()
            if len(row) > 0:
                if row[:3] == 'dlf':
                    header['info'] = row
                    match = re.match(r'dlf-([0-9]+\.[0-9]+) -- ([^\(]+)(?:\((.*)\))?', row).groups()
                    header['dlf-version'] = match[0]
                    header['dlf-component'] = match[1].strip()
                    header['dlf-def-location'] = match[2] if len(match) == 3 else 'builtin'
                else:
                    sep_idx = row.find(':')
                    k = row[:sep_idx].strip()
                    v = row[(sep_idx+1):].strip()
                    if k in header:
                        header[k] += v
                    else:
                        header[k] = v
        try:
            csv_header = next(infile).strip('\n').split('\t')
            units = dict(zip(csv_header, next(infile).strip('\n').split('\t')))
            body = pd.read_csv(infile, sep='\t', names=csv_header)
        except StopIteration:
            units = {}
            body = pd.DataFrame()
    return Dlf(header, units, body)
        
