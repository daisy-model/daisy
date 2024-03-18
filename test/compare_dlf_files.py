'''Compare two dlf files using smallest meaningfull levels'''
import os
import tomllib
import warnings
from functools import reduce

from pint import UnitRegistry
from pint.errors import UndefinedUnitError, DimensionalityError

from dlf import read_dlf

# The default registry is quite large and takes around 0.5s to load. We only need a small part of
# those units, so we just copy the relevant parts from pint/default_en.txt
unit_registry = [
    'milli- = 1e-3  = m-',
    'centi- = 1e-2  = c-',
    'deci- =  1e-1  = d-',
    'kilo- =  1e3   = k-',
    'mega- =  1e6   = M-',
    'giga- =  1e9   = G-',

    'meter = [length] = m = metre',
    'second = [time] = s = sec',
    'gram = [mass] = g',

    'minute = 60 * second = min',
    'hour = 60 * minute = h = hr',
    'day = 24 * hour = d',
    'year = 365.25 * day = a = yr = julian_year',
    'month = year / 12'

    'hectare = 10000 m^2',
]
ureg = UnitRegistry()



def compare_dlf_files(path1, path2,
                      skip_header=frozenset(('VERSION', 'RUN', 'SIMFILE')),
                      precision=1e-8,
                      sml_warn_level=0.001):
    '''Compare two dlf files

    Parameters
    ----------
    path1, path2 : str
      Paths to dlf files

    skip_header: set
      Header keys to exclude from comparison

    precision: float
      Consider two numbers equal if their absolute difference is less than this value

    sml_warn_level: float
      Emit a warning if the absolute difference between two numeric values is larger than this
      value times the corresponding sml

    Returns
    -------
    True if the files compare equal, False otherwise
    '''
    dlf1 = read_dlf(path1)
    dlf2 = read_dlf(path2)

    # Check headers match
    diff_headers = _compare_headers(dlf1.header, dlf2.header, skip_header)
    if len(diff_headers) > 0:
        return False

    diff_units = _compare_units(dlf1.units, dlf2.units)
    if len(diff_units) > 0:
        return False


    sml_names = [os.path.basename(path1), dlf1.header['dlf-component']]
    diff_bodies = _compare_bodies(dlf1.body,
                                  dlf2.body,
                                  dlf1.units,
                                  sml_names,
                                  precision,
                                  sml_warn_level)
    if len(diff_bodies) > 0:
        return False
    return True

def dlf_unit_to_pint_unit(unit):
    '''Convert a unit from a dlf file to a pint unit. Units in dlf files can for example be given as
      kg N/ha = kg nitrogen per hectare
    pint does not understand the nitrogen part, so we need to strip it

    Parameters
    ----------
    unit: str
      dlf unit specification

    Returns
    -------
    pint.Quantity
    
    '''
    if unit in {'', 'DS'}:
        return ureg('dimensionless')
    redefine = {
        ' DM/ha' : '/ha',
        ' N/ha' : '/ha',
        ' C/ha' : '/ha',
    }
    return ureg(reduce(lambda s, kv: s.replace(kv[0], kv[1]), redefine.items(), unit))


def load_smallest_meaningful_level(name):
    '''Load smallest meaningful level (SML) definitions.

    Parameters
    ----------
    name : str
      Name of SML definition

    Returns
    -------
    dict of SML definitions or empty dict if unable to load
    '''
    sml_paths = {
        'harvest' : 'smallest_meaningful_levels/harvest.toml',
        'Field nitrogen' : 'smallest_meaningful_levels/field_nitrogen.toml',
        'Soil nitrogen' : 'smallest_meaningful_levels/soil_nitrogen.toml',
        'Soil water' : 'smallest_meaningful_levels/soil_water.toml',
        'Field water' : 'smallest_meaningful_levels/field_water.toml',
        'Crop' : 'smallest_meaningful_levels/crop.toml',
    }
    try:
        path = os.path.join(os.path.dirname(os.path.realpath(__file__)), sml_paths[name])
        with open(path, 'rb') as infile:
            sml = tomllib.load(infile)
        for k, v in sml.items():
            sml[k] = ureg(v)
        return sml
    except (KeyError, UndefinedUnitError) as e:
        warnings.warn(f'Could not load SML definition for {name}: {e}')
        return {}

def _compare_headers(h1, h2, skip_header):
    diff = []
    for k in (set(h1.keys()) | set(h2.keys())) - skip_header:
        if k not in h1:
            diff.append((k, None, h2[k]))
        elif k not in h2:
            diff.append((k, h1[k], None))
        elif h1[k] != h2[k]:
            diff.append((k, h1[k], h2[k]))
    return diff

def _compare_units(u1, u2):
    diff = []
    for k in set(u1.keys()) | set(u2.keys()):
        if k not in u1:
            diff.append((k, None, u2[k]))
        elif k not in u2:
            diff.append((k, u1[k], None))
        elif u1[k] != u2[k]:
            diff.append((k, u1[k], u2[k]))
    return diff

def _compare_bodies(b1, b2, units, sml_names, precision, sml_warn_level):
    # pylint: disable=too-many-arguments,too-many-locals
    bad_diff = []
    try:
        diff = b1.compare(b2)
        if len(diff) > 0:
            sml_map = {}
            for sml_name in sml_names:
                sml_map = load_smallest_meaningful_level(sml_name)
                if len(sml_map) > 0:
                    break
            for col in diff.columns.levels[0]:
                delta = (diff[col]['self'] - diff[col]['other']).abs().max()
                try:
                    sml = sml_map[col]
                    delta_u = (delta * dlf_unit_to_pint_unit(units[col])).to(sml)
                    if delta_u > sml:
                        bad_diff.append(col, b1[col], b2[col])
                        print(f'ERROR: {delta_u} > {sml} for {col}')
                        print(diff[col])
                    elif delta_u > sml_warn_level * sml:
                        warnings.warn(f'{delta_u} > {sml_warn_level * sml} for {col}')
                except KeyError:
                    # We dont have an SML
                    if delta > precision:
                        bad_diff.append(col, b1[col], b2[col])
                        print(f'ERROR: {delta} > {precision} for {col}')
                        print(diff[col])
                except UndefinedUnitError:
                    error = f'ERROR: Unknown unit {units[col]} for {col}'
                    print(error)
                    bad_diff.append(error, '', '')
                except AttributeError:
                    error = f'ERROR: Unit conversion error {units[col]} for {col}'
                    print(error)
                    bad_diff.append(error, '', '')
                except DimensionalityError:
                    error = f'ERROR: Unit mismatch {units[col]} !~ {sml_map[col]} for {col}'
                    print(error)
                    bad_diff.append(error, '', '')
    except ValueError as e:
        bad_diff.append(e, '', '')
    return bad_diff
