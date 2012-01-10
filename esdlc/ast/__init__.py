'''Provides the `AST` object for parsing a syntax tree from source
code.
'''

__all__ = ['parse', 'load']

import itertools
import esdlc.errors as error
import esdlc.ast.nodes
from esdlc.ast.validator import Validator

class AST(object):
    '''Produces an abstract syntax tree from source code.'''
    
    def __init__(self, tokens):
        self._errors = []
        
        self.expr = []
        keep_going = True
        while keep_going or stmt:
            keep_going = False
            try:
                stmt = self.parse_statement(tokens)
                if stmt:
                    self.expr.append(stmt)
                if tokens and tokens.current.tag != 'eos':
                    self._errors.append(error.InvalidSyntaxError([tokens.current]))
            except error.ESDLSyntaxErrorBase as err:
                keep_going = True
                self._errors.append(err)

            while tokens and tokens.current.tag != 'eos': tokens.move_next()
            while tokens and tokens.current.tag == 'eos': tokens.move_next()
        
        if tokens and tokens.current.tag != 'eos':
            self._errors.append(error.InvalidSyntaxError(tokens.rest))
    
    @property
    def errors(self):
        '''Returns errors found in the system definition.'''
        return [e for e in self._errors if not e.iswarning]
    
    @property
    def warnings(self):
        '''Returns warnings found for the system definition.'''
        return [e for e in self._errors if e.iswarning]
    
    def parse_statement(self, tokens):
        '''Returns exactly one statement from the token stream.'''
        token = tokens.current
        peek = tokens.peek
        if token is None:
            return None
        elif token.tag == 'name':
            func = {
                'from': self.parse_from_stmt,
                'join': self.parse_join_stmt,
                'yield': self.parse_yield_stmt,
                'eval': self.parse_eval_stmt,
                'evaluate': self.parse_eval_stmt,
                'begin': self.parse_begin_stmt,
                'repeat': self.parse_repeat_stmt,
            }.get(token.value)

            if func:
                return func(tokens)
            elif token.value == 'end':
                return ('EndStmt',)
            elif peek and peek.tag == 'assign':
                return self.parse_assign_stmt(tokens)
            else:
                return self.parse_expression(tokens)
        elif token.tag == 'pragma':
            return ('PragmaStmt', token.value)
        elif token.tag == 'comment':
            return ('Comment', token.value)
        elif token.tag == 'eos':
            return None
        else:
            raise error.InvalidSyntaxError([token])
                
    def parse_from_stmt(self, tokens):
        sources = self.parse_groups_or_generators(tokens.move_next())
        
        if not tokens.current or tokens.current.value != 'select':
            raise error.ExpectedSelectError([tokens.current])
        
        destinations = self.parse_sized_groups(tokens.move_next())
        operators = self.parse_using(tokens)

        return ('FromStmt', sources, destinations, operators)

    def parse_join_stmt(self, tokens):
        sources = self.parse_groups_or_generators(tokens.move_next())
        
        if not tokens.current or tokens.current.value != 'into':
            raise error.ExpectedIntoError([tokens.current])
        
        destinations = self.parse_sized_groups(tokens.move_next())
        operators = self.parse_using(tokens)
        
        return ('JoinStmt', sources, destinations, operators)

    def parse_yield_stmt(self, tokens):
        sources = self.parse_groups(tokens.move_next())
        return ('YieldStmt', sources)

    def parse_eval_stmt(self, tokens):
        sources = self.parse_groups(tokens.move_next())
        operators = self.parse_using(tokens)
        return ('EvalStmt', sources, operators)

    def parse_using(self, tokens):
        if tokens.current and tokens.current.value == 'eos':
            return []
        elif tokens.current and tokens.current.value == 'using':
            return self.parse_operators(tokens.move_next())
        else:
            raise error.ExpectedUsingError([tokens.current])

    def parse_begin_stmt(self, tokens):
        stmt = tokens.current
        tokens.move_next()
        if not tokens or tokens.current.tag != 'name':
            raise error.ExpectedBlockNameError([stmt])
        name = tokens.current.value
        while tokens and tokens.current.tag != 'eos': tokens.move_next()
        return ('BeginStmt', name)

    def parse_repeat_stmt(self, tokens):
        tokens.move_next()
        if not tokens:
            raise error.ExpectedRepeatCountError([tokens.current])
        count = self.parse_expression(tokens)
        while tokens and tokens.current.tag != 'eos': tokens.move_next()
        return ('RepeatStmt', count)

    def parse_groups(self, tokens):
        groups = []
        group_name = self.parse_expression(tokens)
        
        while group_name:
            groups.append(group_name)

            if not tokens or tokens.current.value != ',':
                break

            group_name = self.parse_expression(tokens.move_next())

        return groups

    def parse_sized_groups(self, tokens):
        pass

    def parse_groups_or_generators(self, tokens):
        pass

    def parse_operators(self, tokens):
        pass

    def parse_parameters(self, tokens):
        token = tokens.current
        peek = tokens.peek

        if not token or token.tag == 'close':
            return None
        elif token.tag != 'name':
            raise error.InvalidParameterNameError([token])

        parameters = []
        while token and token.tag == 'name':
            if peek.value == '=':
                arg = self.parse_expression(tokens.move_next().move_next())
                parameters.append((token.value, arg))
            elif peek.value == ',':
                parameters.append((token.value, None))
                tokens.move_next()
            elif peek.value == ')':
                parameters.append((token.value, None))
            else:
                raise error.ExpectedParameterValueError([peek])
            
            if not tokens or tokens.current.value == ')':
                break
            
            token = tokens.move_next().current
            peek = tokens.peek

        return parameters

    def parse_assign_stmt(self, tokens):
        dest = tokens.current
        tokens.move_next()
        if not tokens or tokens.current.value != '=':
            raise error.InvalidSyntaxError([tokens.current or dest])

        src = self.parse_expression(tokens.move_next())
        if not src:
            raise error.InvalidSyntaxError([tokens.current or dest])
        return ('=', dest.value, src)

    CONSTANT_VALUES = {'true': True, 'false': False, 'null': None, 'none': None}

    def parse_operand(self, tokens):
        token = tokens.current
        peek = tokens.peek
        if not token:
            return None

        if token.tag == 'name':
            name = tokens.current.value
            tokens.move_next()
            if name in self.CONSTANT_VALUES:
                return ('Constant', self.CONSTANT_VALUES[name])
            else:
                return ('Name', name)
        elif token.tag == 'number':
            tokens.move_next()
            try:
                return ('Number', float(token.value))
            except:
                raise error.InvalidNumberError([token])
        elif token.tag == 'open':
            open_bracket = token
            op = self.parse_expression(tokens.move_next())
            token = tokens.current
            if not token or token.tag != 'close':
                raise error.UnmatchedBracketError([open_bracket], open_bracket.value)
            tokens.move_next()
            return op
        else:
            return None

    def parse_expression(self, tokens):
        if not tokens:
            return None

        if tokens.current.value in '+-':
            expr = [None]
        else:
            operand = self.parse_operand(tokens)
            if not operand: return None
            expr = [operand]

        while tokens:
            op_token = tokens.current
            if not op_token or op_token.tag not in ('open', 'operator'):
                break

            op = op_token.value
            if op == '(':
                expr.append('CallFunc')
                expr.append(self.parse_parameters(tokens.move_next()))
                if not tokens: raise error.InvalidFunctionCallError([op_token])
                if tokens.current.value != ')': raise error.InvalidFunctionCallError([tokens.current])
                tokens.move_next()
            elif op == '[':
                expr.append('GetElement')
                expr.append(self.parse_expression(tokens.move_next()))
                if not tokens: raise error.UnmatchedBracketError([op_token], '[')
                if tokens.current.value != ']': raise error.UnmatchedBracketError([tokens.current], '[')
                tokens.move_next()
            else:
                expr.append(op)
                tokens.move_next()
                if tokens and tokens.current.tag == 'operator' and tokens.current.value in '+-':
                    operand = None
                else:
                    operand = self.parse_operand(tokens)
                    if not operand: raise error.InvalidSyntaxError([op_token])
                expr.append(operand)

        def _reduce(expr, match):
            i = 0
            while i + 3 <= len(expr):
                left, op, right = expr[i:i+3]
                if op in match:
                    expr[i:i+3] = [(op, left, right)]
                else:
                    i += 2

        def _reduce_unary(expr, match):
            i = 0
            while i + 3 <= len(expr):
                left, op, right = expr[i:i+3]
                if left is None and op in match:
                    expr[i:i+3] = [(op, left, right)]
                else:
                    i += 2
        
        def _reduce_reverse(expr, match):
            i = len(expr) - 3
            while i >= 0:
                left, op, right = expr[i:i+3]
                if op in match:
                    expr[i:i+3] = [(op, left, right)]
                i -= 2
        
        _reduce(expr, ('.', 'CallFunc', 'GetElement'))
        _reduce_unary(expr, '+-')
        _reduce(expr, '%')
        _reduce_reverse(expr, '^')
        _reduce(expr, '*/')
        _reduce(expr, '+-')
        

        assert len(expr) == 1
        return expr[0]

    def format(self):
        '''Returns a formatted string representation of the AST.
        '''
        return str(self)

    def __str__(self):
        '''Returns an abbreivated string representation of the AST.
        '''
        def _fmt(node, result):
            if node is None:
                pass
            elif isinstance(node, list):
                result.append('[')
                for item in node:
                    _fmt(item, result)
                    result.append(',')
                if result[-1] == ',': result.pop()
                result.append(']')
            elif isinstance(node, tuple):
                if node[0] == 'Name':
                    result.append(str(node[1]))
                elif node[0] == 'Number':
                    result.append(str(node[1]))
                elif node[0] == 'Constant':
                    result.append('Constant{%s}' % (node[1] if node[1] is not None else 'None'))
                else:
                    _fmt(node[0], result)
                    result.append('{')
                    for item in node[1:]:
                        _fmt(item, result)
                        result.append(',')
                    if result[-1] == ',': result.pop()
                    result.append('}')
            else:
                result.append(str(node))
            return result

        return ';'.join(''.join(_fmt(stmt, [])) for stmt in self.expr)


def parse(source):
    '''Loads an `AST` instance from the contents of `source`.'''
    from esdlc.ast.lexer import TokenReader
    tokens = TokenReader(esdlc.ast.lexer.tokenise(source))
        
    self = AST(tokens)
    return self

def load(path):
    '''Loads an `AST` instance from the contents of the file at
    `path`.
    '''
    with open(path) as src:
        return parse(src)

import lexer
a = AST(lexer.TokenReader(lexer.tokenise("a=1+2^-test(v=3*4)"), True))
