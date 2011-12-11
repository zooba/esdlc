'''Converts a semantic model to compilable C++ code
using C++ AMP.

'''
from collections import namedtuple
import itertools
import os.path
import sys
from warnings import warn

import built_in_template
import opening_template
import termination_template
import yield_template
import re
import subprocess

ILLEGAL_VARIABLE_NAMES = frozenset((
    'and', 'and_eq', 'asm', 'auto', 'bitand', 'bitor', 'bool', 'break', 'case',
    'catch', 'char', 'class', 'compl', 'const', 'const_cast', 'continue',
    'default', 'delete', 'do', 'double', 'dynamic_cast', 'else', 'enum',
    'explicit', 'extern', 'false', 'float', 'for', 'friend', 'goto', 'if',
    'inline', 'int', 'long', 'mutable', 'namespace', 'new', 'not', 'not_eq',
    'operator', 'or', 'or_eq', 'private', 'protected', 'public', 'register',
    'reinterpret_cast', 'return', 'short', 'signed', 'sizeof', 'static',
    'static_cast', 'struct', 'switch', 'template', 'this', 'throw', 'true',
    'try', 'typedef', 'typeid', 'typename', 'union', 'unsigned', 'using',
    'virtual', 'void', 'volatile', 'wchar_t', 'while', 'xor', 'xor_eq',

    'std', 'esdl',
))

def _format(lines):
    indented_lines = []
    indent = 0
    for line in lines:
        if not line:
            indented_lines.append('    ' * indent)
        else:
            if line.strip().startswith('}'): indent -= 1
            indented_lines.append('    ' * indent + line)
            if line.strip().endswith('{'): indent += 1
    return '\n'.join(indented_lines)



class Parameter(object):
    def __init__(self, name=None, cast=None, default=None, templated=False):
        self.name = name
        self.cast = cast
        self.default = default
        self.templated = templated

    def __str__(self):
        return ('%s=<%s>%s' if self.templated else '%s=(%s)%s') % (self.name, self.cast, self.default)

    def __repr__(self):
        return 'Parameter(name=%r, cast=%r, default=%r, templated=%r)' % (self.name, self.cast, self.default, self.templated)

class Function(object):
    def __init__(self, name=None, parameters=None):
        self.name = name
        self.parameters = parameters

    def __str__(self):
        return '%s(%s)' % (self.name, ', '.join(str(i) for i in self.parameters))

    def __repr__(self):
        return 'Function(name=%r, parameters=%r)' % (self.name, self.parameters)

def _inherit_property(name):
    pname = '_' + name
    def _get(s):
        return getattr(s.global_scope, name) if (s.global_scope and s.global_scope is not s) else getattr(s, pname)
    def _set(s, v):
        if s.global_scope and s.global_scope is not s: setattr(s.global_scope, name, v)
        else: setattr(s, pname, v)
    return property(_get, _set)

class EmitterScope(object):
    def __init__(self, global_scope=None):
        assert global_scope is None or isinstance(global_scope, EmitterScope)
        g = global_scope
        self.global_scope = global_scope or self

        self.block_name = None
        self.block_count = 0
        self.block_index = 0
        self.allocated_groups = set()
        self.allocated_variables = set()
        self.group_alias = {}
        self.valid_constants = {}

        self._default_group = None
        self._default_evaluator = None
        self._selector = None
        self._repeat_id = 0
        self._store_id = 0

        self._anonymous_variables = g.anonymous_variables if g else {}
        self.default_group_yielded = g.default_group_yielded if g else False
        self.source_path = g.source_path if g else None
        self.inclusions = g.inclusions if g else []
        self.known_operators = g.known_operators if g else []
        self.known_functions = g.known_functions if g else []
        self.known_evaluators = g.known_evaluators if g else []
        self.known_generators = g.known_generators if g else []
        self.known_joiners = g.known_joiners if g else []
        self.uninitialised_variables = g.uninitialised_variables if g else set()
        self.yielded_groups = g.yielded_groups if g else set()
        self.enable_cv = g.enable_cv if g else False

    default_group = _inherit_property('default_group')
    default_evaluator = _inherit_property('default_evaluator')
    selector = _inherit_property('selector')
    repeat_id = _inherit_property('repeat_id')
    store_id = _inherit_property('store_id')
    anonymous_variables = _inherit_property('anonymous_variables')

    def has(self, value, key):
        return value in getattr(self, key) or (self.global_scope is not None and value in getattr(self.global_scope, key))

    def safe_variable(self, name):
        if name in self.anonymous_variables:
            return self.anonymous_variables[name]

        name = name.replace('.', '::')
        m = re.match('^[a-z][a-z0-9_:]*$', name)
        if not m or name in ILLEGAL_VARIABLE_NAMES:
            new_name = '_anon_%d' % len(self.anonymous_variables)
            self.anonymous_variables[name] = new_name
            return new_name

        return name

    def _split_with_nesting(self, source, split_at=",", nest_open="([{", nest_close=")]}", nesting_start=0):
        word = ''
        nesting = nesting_start
        for c in source:
            if c in split_at and nesting == 0:
                yield word
                word = ''
                continue
            elif c in nest_open:
                nesting += 1
                if nesting == 0 and word:
                    yield word
                    word = ''
                    continue
                word += c
            elif c in nest_close:
                if nesting != 0: word += c
                nesting -= 1
                if nesting == nesting_start: break
            else:
                word += c

        if word:
            yield word

    def _parse_function(self, defn):
        parts = list(self._split_with_nesting(defn, nesting_start=-1))
        params = []
        func = Function(name=parts.pop(0), parameters=params)
        for p in parts:
            pname, _, default = p.partition('=')
            param = Parameter(pname.strip(), None, default, False)
            if default[0] == '(':
                param.cast, _, param.default = default[1:].partition(')')
            elif default[0] == '<':
                param.cast, _, param.default = default[1:].partition('>')
                param.templated = True
            params.append(param)
        return func

    def _find_function(self, name, params, source):
        candidates = [i for i in source if i.name == name]

        if candidates and len(candidates) > 1:
            candidates = [i for i in candidates if all(any(j.name == k.name for j in i.parameters) for k in params)]

        if not candidates:
            warn("Cannot find function %s" % name)
            return None
        elif len(candidates) > 1:
            warn("Cannot resolve function %s. Could be:\n  %s" % (name, '\n  '.join(str(i) for i in candidates)))
            return None
        else:
            return candidates[0]

    def include(self, filename):
        assert isinstance(filename, str)

        self.inclusions.append(filename)

        if filename[0] == '<':
            path = os.path.join(self.search_path, filename.strip('<>'))
        else:
            path = os.path.join(self.source_path, filename.strip('"\''))

        if not os.path.exists(path):
            return
        
        with open(path, 'rU') as f:
            assert isinstance(f, file)
            for line in f:
                assert isinstance(line, str)
                line = line.lower()
                if not line.startswith('// esdl'): continue
                command = line[8:line.index(':')].lower()
                defn = line[line.index(':')+1:].strip()
                if command == 'generator':
                    self.known_generators.append(self._parse_function(defn))
                elif command == 'operator':
                    self.known_operators.append(self._parse_function(defn))
                elif command == 'evaluator':
                    self.known_evaluators.append(self._parse_function(defn))
                elif command == 'function':
                    self.known_functions.append(self._parse_function(defn))
                elif command == 'joiner':
                    self.known_joiners.append(self._parse_function(defn))
                else:
                    warn("Unknown command in %s: %s" % (os.path.split(path)[1], line))


    def _expr_to_string(self, expr):
        if isinstance(expr, str):
            return expr
        elif expr is None:
            return None
        elif expr.tag == 'variable':
            if expr.constant:
                if expr.value is True: return "true"
                elif expr.value is False: return "false"
                elif expr.value is None: return "nullptr"
                elif isinstance(expr.value, float): return '%ff' % expr.value
                else: return str(expr.value)
            return self.safe_variable(expr.name)
        elif expr.tag == 'variableref':
            return self._expr_to_string(expr.id)
        elif expr.tag == 'groupref':
            name = expr.id.name
            name = self.group_alias.get(name) or self.global_scope.group_alias.get(name) or name
            return self.safe_variable(name)
        elif expr.tag == 'function':
            return self._func_to_string(expr)
        
        elif expr.tag == 'binaryop':
            return '(%s%s%s)' % (self._expr_to_string(expr.left), expr.op, self._expr_to_string(expr.right))
        elif expr.tag == 'unaryop':
            return '(%s%s)' % (expr.op, self._expr_to_string(expr.right))
        else:
            warn("Cannot convert %s to string: %s" % (expr.tag, expr))
            return ''

    def _funcobj_to_string(self, func, params, leading_parameters=None):
        parts = [func.name, '(']
        if leading_parameters:
            for p in leading_parameters:
                parts.append(self._expr_to_string(p))
                parts.append(', ')

        for p in func.parameters:
            try:
                pvalue = params[p.name]
                if pvalue is None:
                    if self.has(p.name, 'allocated_variables'):
                        value = self.safe_variable(p.name)
                    elif p.cast == 'bool':
                        value = 'true'
                    else:
                        value = self.safe_variable(p.name)
                        self.uninitialised_variables.add(p.name)
                else:
                    value = self._expr_to_string(pvalue)
            except KeyError:
                value = p.default

            if p.templated:
                value = self.valid_constants.get(value) or self.global_scope.valid_constants.get(value) or value
                if p.cast == 'bool':
                    if value.lower() == 'true':
                        parts.append('std::true_type()')
                        parts.append(', ')
                    elif value.lower() == 'false':
                        parts.append('std::false_type()')
                        parts.append(', ')
                    else:
                        warn("Cannot use '%s' as templated bool parameter." % value)
                elif p.cast == 'int' or p.cast == 'unsigned int':
                    parts.append('std::integral_constant<%s, %d>()' % (p.cast, value))
                    parts.append(', ')
                else:
                    warn("Invalid parameter for templating: %s" % p)
            else:
                parts.extend(('(', p.cast, ')', value, ', '))
        if parts[-1] == ', ': parts.pop()
        parts.append(')')
        return ''.join(parts)

    def _func_to_string(self, expr, leading_parameters=None):
        if expr.name == '_call':
            name = [i for i in expr.parameters if i.name == '_function'][0].value
            args = [i for i in expr.parameters if i.name != '_function']

            if name.tag == 'variable':
                name = name.name
            elif name.tag == 'variableref':
                name = name.id.name
            else:
                warn("Cannot write function for %s: %s" % (name.tag, name))
                return ''

            func = self._find_function(name, (i.name for i in args), self.known_functions)
            if func:
                return self._funcobj_to_string(func, expr.parameter_dict)
            return None

        elif expr.name == '_alias':
            dest, src = expr.parameter_dict['_destination'], expr.parameter_dict['_source']
            dest = self._expr_to_string(dest)
            src = self._expr_to_string(src)
            self.group_alias[dest] = src
            self.allocated_variables.add(dest)
            self.uninitialised_variables.discard(dest)
            return ''
        elif expr.name == '_assign':
            dest, src = expr.parameter_dict['_destination'], expr.parameter_dict['_source']
                
            if dest.tag == 'variableref':
                dest = self._expr_to_string(dest)
                if src.tag == 'variableref' and src.id.constant:
                    self.valid_constants[dest] = src.id.value
                if self.has(dest, 'allocated_variables'):
                    src = '(decltype(' + dest + '))' + self._expr_to_string(src)
                else:
                    self.allocated_variables.add(dest)
                    self.uninitialised_variables.discard(dest)
                    dest = 'auto ' + dest
                    src = self._expr_to_string(src)
            else:
                dest = self._expr_to_string(dest)
                src = self._expr_to_string(src)
            return '%s = %s;' % (dest, src)
        elif expr.name == '_getattrib':
            src, attrib = expr.parameter_dict['_source'], expr.parameter_dict['_attrib']
            return '%s.%s' % (self._expr_to_string(src), self._expr_to_string(attrib))
        elif expr.name == '_getindex':
            src, index = expr.parameter_dict['_source'], expr.parameter_dict['_index']
            return '%s[%s]' % (self._expr_to_string(src), self._expr_to_string(index))

        warn("Unhandled function call: %s" % expr.name)

    def _operator_to_string(self, op, source, store_id):
        name = self._expr_to_string(op.func.parameter_dict['_function'])
        name2 = name.replace('.', '::')
        params = [i for i in op.func.parameter_dict.iterkeys() if i != '_function']
        
        func = (self._find_function(name, params, self.known_operators) or 
                self._find_function(name2, params, self.known_operators))

        if not func:
            return ''

        return self._funcobj_to_string(func, op.func.parameter_dict, [source])

    def _generator_to_string(self, generator):
        name = self._expr_to_string(generator.parameter_dict['_function'])
        name2 = name.replace('.', '::')
        params = [i for i in generator.parameter_dict.iterkeys() if i != '_function']

        func = (self._find_function(name, params, self.known_generators) or
                self._find_function(name2, params, self.known_generators))

        if not func:
            return ''

        return self._funcobj_to_string(func, generator.parameter_dict)

    def _joiner_to_string(self, joiner, groups):
        name = self._expr_to_string(joiner.parameter_dict['_function'])
        name2 = name.replace('.', '::')
        params = [i for i in joiner.parameter_dict.iterkeys() if i != '_function']
        
        func = (self._find_function(name, params, self.known_joiners) or 
                self._find_function(name2, params, self.known_joiners))

        if not func:
            return ''

        group_names = ['esdl::merge(%s)' % self._expr_to_string(i) for i in groups]
        return self._funcobj_to_string(func, joiner.parameter_dict, group_names)


    def _store_to_lines(self, stmt):
        lines = []
        ending = []
        id = self.store_id
        self.store_id += 1

        op = stmt.source
        op_stack = []
        while op.tag not in set(('merge', 'join')):
            op_stack.append(op)
            op = op.source
        
        add_evaluator = False
        if op.tag == 'merge':
            if len(op.sources) == 1:
                group = op.sources[0]
                if group.tag == 'function':
                    lines.append('auto _src_%d = %s;' % (id, self._generator_to_string(group)))
                    add_evaluator = True
                else:
                    groupname = self._expr_to_string(group)
                    lines.append('auto _src_%d = esdl::merge(%s);' % (id, groupname))
            else:
                groupname = self._expr_to_string(op.sources[0])
                line = 'decltype(%s) _merge_%d[] = { %s' % (groupname, id, groupname)
                need_free = 1
                for group in op.sources[1:]:
                    groupname = self._expr_to_string(group)
                    if group.tag == 'function':
                        lines.append(line + ' };')
                        line = None
                        lines.append('auto _src_%d = esdl::merge(_merge_%d, %s);' %
                                        (id, id, groupname))
                        break
                    else:
                        line += ', ' + self._expr_to_string(group)
                        need_free += 1
                
                ending.append(('for (int _i_%(id)d = 0; _i_%(id)d < %(nf)d; ++_i_%(id)d) ' +
                               '_merge_%(id)d[_i_%(id)d].reset();') % { 'id': id, 'nf': need_free })
                if line:
                    lines.append(line + ' };')
                    lines.append('auto _src_%d = esdl::merge(_merge_%d);' % (id, id))
        else:
            groups = op.sources
            op = op_stack.pop()
            lines.append('auto _src_%d = %s;' % (id, self._joiner_to_string(op.func, groups)))
        
        gen = '_src_%d' % id
        while op_stack:
            op = op_stack.pop()
            gen = self._operator_to_string(op, gen, id)
        
        lines.append('auto _gen_%d = %s;' % (id, gen))
        
        if not add_evaluator or not self.default_evaluator:
            for group in stmt.destinations:
                groupname = self._expr_to_string(group.id)
                if self.has(groupname, 'allocated_groups'):
                    line = '%s = _gen_%d(' % (groupname, id)
                else:
                    line = 'auto %s = _gen_%d(' % (groupname, id)
                    self.allocated_groups.add(groupname)
                    self.uninitialised_variables.discard(groupname)
            
                if group.limit:
                    lines.append(line + '(int)' + self._expr_to_string(group.limit) + ');')
                else:
                    lines.append(line + ');')
                    break
        else:
            for i, group in enumerate(stmt.destinations):
                tempname1 = '_noeval_%d_%d' % (id, i)
                line = 'auto %s = _gen_%d(' % (tempname1, id)
                if group.limit:
                    line += '(int)' + self._expr_to_string(group.limit)
                lines.append(line + ');')

                part1, _, part2 = self.default_evaluator.partition('(')
                if part2.strip() == ')':
                    evaluator = '%s(%s)' % (part1, tempname1)
                else:
                    evaluator = '%s(%s, %s' % (part1, tempname1, part2)

                groupname = self._expr_to_string(group.id)
                if self.has(groupname, 'allocated_groups'):
                    lines.append('%s = %s.evaluate_using(%s);' % (groupname, tempname1, evaluator))
                else:
                    lines.append('auto %s = %s.evaluate_using(%s);' % (groupname, tempname1, evaluator))
                    self.allocated_groups.add(groupname)
                    self.uninitialised_variables.discard(groupname)

                if not group.limit:
                    break
            
        lines.extend(ending)
        return lines

    def _write_block(self, statements):
        lines = []
        ending = []

        if self.block_name and self.block_count > 2:
            if self.selector:
                lines.append('if (esdl::equals(_current_block, L"%s")) {' % self.block_name)
            else:
                lines.append('if ((_iteration %% %d) == %d) {' % (self.global_scope.block_count-1, self.block_index))
            ending.insert(0, '} else')

        for stmt in statements:
            lines.append('// %s' % stmt)
            if stmt.tag == 'pragma':
                if stmt.text.startswith('include '):
                    filename = stmt.text[8:]
                    self.include(filename)
                elif stmt.text.startswith('evaluator '):
                    self.default_evaluator = stmt.text[10:]
                elif stmt.text.startswith('selector '):
                    self.selector = stmt.text[9:]
                elif stmt.text.startswith('yield '):
                    groupname = stmt.text[6:].strip()
                    if self.default_group:
                        warn("Cannot override existing default group.")
                    else:
                        self.default_group = groupname
                        self.default_group_yielded = False
                        
                else:
                    pass # ignore other pragmas silently

            elif stmt.tag == 'repeatblock':
                self.repeat_id += 1
                id = self.repeat_id
                lines.append('int _repeat_limit_%d = (int)%s;' % (id, self._expr_to_string(stmt.count)))
                lines.append('for(int _repeat_%d = 0; _repeat_%d < _repeat_limit_%d; ++_repeat_%d) {' % (id, id, id, id))
                lines.extend(self._write_block(stmt.statements))
                lines.append('}')

            elif stmt.tag == 'function':
                lines.append(self._func_to_string(stmt))

            elif stmt.tag == 'store':
                if self.enable_cv:
                    lines.append('CvEnterSpanW(_cv_series, &_cv_span, L"%s");' % stmt)
                lines.extend(self._store_to_lines(stmt))
                if self.enable_cv:
                    lines.append('esdl::acc.flush(); esdl::acc.wait();')
                    lines.append('CvLeaveSpan(_cv_span);')
                lines.append('')

            elif stmt.tag == 'evalstmt':
                evaluator = None
                if stmt.evaluators:
                    evaluator = stmt.evaluators[0]
                    # TODO: Instantiate evaluator
                    if len(stmt.evaluators) > 1:
                        warn("Multiple evaluators are not supported.")

                for group in stmt.sources:
                    groupname = self._expr_to_string(group)
                    # TODO: Assign evaluator
                    lines.append('%s.evaluated = false;' % groupname)

            elif stmt.tag == 'yieldstmt':
                for group in stmt.sources:
                    if group.tag == 'groupref': group = group.id
                    name = group.name
                    safename = self.safe_variable(name)
                    if not self.default_group:
                        self.default_group = name
                    
                    if self.enable_cv:
                        lines.append('CvEnterSpanW(_cv_series, &_cv_span, L"YIELD %s");' % name)

                    if name == self.default_group:
                        lines.extend(yield_template.get(name, safename, True, not self.default_group_yielded))
                        self.default_group_yielded = True
                        self.yielded_groups.add(safename)
                    else:
                        lines.extend(yield_template.get(name, safename, False, self.has(safename, 'yielded_groups')))
                        self.yielded_groups.add(safename)

                    if self.enable_cv:
                        lines.append('CvLeaveSpan(_cv_span);')


        return lines + ending

def emit(model, out=sys.stdout, optimise_level=0, profile=False):
    '''Converts the provided model to C++ code using C++ AMP.
    '''
    if not os.path.exists(model.source_file):
        return None, None

    global_scope = EmitterScope()
    global_scope.block_count = len(model.block_names)
    global_scope.uninitialised_variables.update(model.variables.iterkeys())
    global_scope.enable_cv = profile
    global_scope.source_path = os.path.split(model.source_file)[0]
    global_scope.search_path = os.path.join(os.path.split(__file__)[0], 'lib')
    global_scope.include("<esdl.h>")
    lines = []

    if global_scope.enable_cv:
        lines.extend([
            'PCV_PROVIDER _cv_provider = nullptr;',
            'PCV_MARKERSERIES _cv_series = nullptr;',
            'PCV_SPAN _cv_span = nullptr;',
            'CvInitProvider(&CvDefaultProviderGuid, &_cv_provider);',
            'CvCreateMarkerSeriesW(_cv_provider, L"esdlc", &_cv_series);',
        ])

    lines.extend(global_scope._write_block(model.blocks[model.INIT_BLOCK_NAME]))

    if not global_scope.default_group_yielded:
        if global_scope.default_group:
            lines.extend(yield_template.get(global_scope.default_group, self.safe_variable(global_scope.default_group), True, True, True))
            global_scope.default_group_yielded = True
        else:
            warn('Default group not specified in initialisation. Statistics and fitness termination are not available.')

    lines.extend(termination_template.get(global_scope.default_group))

    block_i = 0
    for block_name in model.block_names:
        if block_name == model.INIT_BLOCK_NAME: continue
        local_scope = EmitterScope(global_scope)
        local_scope.block_name = block_name
        local_scope.block_index = block_i
        lines.extend(local_scope._write_block(model.blocks[block_name]))
        block_i += 1
    
    if len(model.block_names) > 2:
        lines.extend(('{', 'std::wcerr << L"Unrecognised block: " << _current_block;', 'return 2;', '}'))

    preamble = built_in_template.get(model.source_file)

    preamble.extend('#include ' + s for s in global_scope.inclusions)
    
    preamble.extend(opening_template.get((s, global_scope.safe_variable(s)) for s in global_scope.uninitialised_variables))

    lines.append('}')

    if global_scope.enable_cv:
        lines.append('CvReleaseMarkerSeries(_cv_series);')
        lines.append('CvReleaseProvider(_cv_provider);')

    lines.append('}')
    formatted_lines = _format(preamble + lines)
    if out:
        out.write(formatted_lines)

    def _compile(path):
        if os.path.splitext(path)[1].lower() != '.exe':
            return
        exepath, path = path, os.path.splitext(path)[0] + '.cpp'
        os.remove(path)
        os.rename(exepath, path)

        here = os.path.split(__file__)[0]
        if not os.path.exists(here):
            return

        cmd = ['cl', '/Ox', '/GF', '/GR-', '/GL', '/EHsc', '/MP',
               '/I' + os.path.expandvars(r'%VSINSTALLDIR%Common7\IDE\Extensions\Microsoft\Concurrency Visualizer\SDK\Native\Inc'),
               '/I' + os.path.join(here, 'lib'),
               path,
               os.path.join(here, 'lib', 'rng.cpp'),
               os.path.join(here, 'lib', 'utility.cpp'),
               '/link',
               'advapi32.lib']
        subprocess.call(cmd)

    return lines, { '__compile': _compile }

if __name__ == '__main__':
    source = ['if a {', 'b + c', '} else {', 'd', '}', 'e']
    expected = 'if a {\n    b + c\n} else {\n    d\n}\ne'
    actual = _format(source)
    assert expected == actual, '%s != %s' % (expected, actual)

    s = EmitterScope()
    for source, expected in [
        ('variable', 'variable'), ('abc123', 'abc123'), ('abc_123', 'abc_123'), ('123abc', '_anon_0'),
        ('abc+123', '_anon_1'), ('123abc', '_anon_0')
    ]:
        actual = s.safe_variable(source)
        assert expected == actual, '%s != %s' % (expected, actual)
