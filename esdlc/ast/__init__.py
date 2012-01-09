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
        stmt, err = self.parse_statement(tokens)
        while stmt or err:
            if stmt:
                self.expr.append(stmt)
            elif err:
                self._errors.append(err)
            while tokens.current and tokens.current.tag != 'eos':
                tokens.move_next()
            stmt, err = self.parse_statement(tokens)
        
        if tokens.current and tokens.current.tag != 'eos':
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
            return None, None
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
                stmt, err = func(tokens)
                return stmt, err
            elif token.value == 'end':
                return ('EndStmt',), None
            elif peek and peek.tag == 'open':
                return self.parse_call_func(tokens)
            elif peek and peek.tag == 'operator' and peek.value == '=':
                return self.parse_assign_stmt(tokens)
            else:
                return None, error.InvalidSyntaxError([token])
        elif token.tag == 'pragma':
            return ('PragmaStmt', token.value), None
        elif token.tag == 'comment':
            return ('Comment', token.value), None
        elif token.tag == 'eos':
            return None, None
        else:
            return None, error.InvalidSyntaxError([token])
                
    def parse_from_stmt(self, tokens):
        sources = self.parse_groups_or_generators(tokens.move_next())
        
        if not tokens.current or tokens.current.value != 'select':
            return None, error.ExpectedSelectError([tokens.current])
        
        destinations = self.parse_sized_groups(tokens.move_next())
        
        operators, err = self.parse_using(tokens)
        if err:
            return None, err
        
        return ('FromStmt', sources, destinations, operators), None

    def parse_join_stmt(self, tokens):
        sources = self.parse_groups_or_generators(tokens.move_next())
        
        if not tokens.current or tokens.current.value != 'into':
            return None, error.ExpectedIntoError([tokens.current])
        
        destinations = self.parse_sized_groups(tokens.move_next())
        
        operators, err = self.parse_using(tokens)
        if err:
            return None, err
        
        return ('JoinStmt', sources, destinations, operators), None

    def parse_yield_stmt(self, tokens):
        sources = self.parse_groups(tokens.move_next())
        return ('YieldStmt', sources), None

    def parse_eval_stmt(self, tokens):
        sources = self.parse_groups(tokens.move_next())

        operators, err = self.parse_using(tokens)
        if err:
            return None, err

        return ('EvalStmt', sources, operators), None

    def parse_using(self, tokens):
        if tokens.current and tokens.current.value == 'eos':
            return [], None
        elif tokens.current and tokens.current.value == 'using':
            return self.parse_operators(tokens.move_next()), None
        else:
            return None, error.ExpectedUsingError([tokens.current])

    def parse_begin_stmt(self, tokens):
        if not peek or peek.tag != 'name':
            return None, error.ExpectedBlockNameError([token])
        return ('BeginStmt', peek.value), None

    def parse_repeat_stmt(self, tokens):
        if not peek:
            return None, error.ExpectedRepeatCountError([token])
        count, err = self.parse_expression(tokens.move_next())
        if err:
            return None, err
        return ('RepeatStmt', count), None

    def parse_groups(self, tokens):
        pass

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
            return None, None
        elif token.tag != 'name':
            return None, error.InvalidParameterNameError([token])

        parameters = []
        while token and token.tag == 'name':
            if peek.value == '=':
                arg, err = self.parse_expression(tokens.move_next().move_next())
                if err: return None, err
                parameters.append((token.value, arg))
            elif peek.value == ',' or peek.value == ')':
                parameters.append((token.value, None))
            else:
                return None, error.ExpectedParameterValueError([peek])
            token = tokens.move_next().current
            peek = tokens.peek

        return parameters, None

    def parse_call_func(self, tokens):
        token = tokens.current
        peek = tokens.peek

        if not token or not peek:
            return None, None
        elif token.tag != 'name':
            return None, error.InvalidFunctionCallError([token])
        elif peek.value != '(':
            return None, error.InvalidFunctionCallError([peek])

        parameters, err = self.parse_parameters(tokens.move_next().move_next())
        if err: return None, err
        tokens.move_next()
        return ('CallFunc', token.value, parameters), None

    def parse_assign_stmt(self, tokens):
        dest = tokens.current
        tokens.move_next()
        if not tokens or tokens.current.value != '=':
            return None, error.InvalidSyntaxError([tokens.current or dest])
        src, err = self.parse_expression(tokens.move_next())
        if err:
            return None, err
        return ('CallFunc', '_assign', [('dest', dest), ('src', src)]), None

    CONSTANT_VALUES = {'true': True, 'false': False, 'null': None, 'none': None}

    def parse_operand(self, tokens):
        token = tokens.current
        negate = False
        while token and token.value == '-':
            negate = not negate
            token = tokens.move_next().current
        
        peek = tokens.peek
        if not token:
            return None, None

        if token.tag == 'name':
            text = tokens.current.value
            if text in self.CONSTANT_VALUES:
                tokens.move_next()
                op = ('Constant', values[text])
            elif peek and peek.value == '(':
                op, err = self.parse_call_func(tokens)
                if err:
                    return None, err
            elif peek and peek.value == '[':
                op, err = self.parse_get_element(tokens)
                if err:
                    return None, err
            else:
                tokens.move_next()
                op = ('Name', text)
            
            if negate:
                return ('-', None, op), None
            else:
                return op, None

        elif token.tag == 'number':
            tokens.move_next()
            try:
                return ('Number', float(token.value) * (-1 if negate else 1)), None
            except:
                return None, error.InvalidNumberError([token])
        elif token.tag == 'open':
            open_bracket = token
            op, err = self.parse_expression(tokens.move_next())
            if err:
                return None, err
            token = tokens.current
            if not token or token.tag != 'close':
                return None, error.UnmatchedBracketError([open_bracket], open_bracket.value)
            tokens.move_next()
            if negate:
                return ('-', None, op), None
            else:
                return op, None
        else:
            return None, None

    OPERATOR_RANK = { '+': 1, '-': 1, '*': 2, '/': 2, '^': 3, '%': 4 }

    def parse_expression(self, tokens):
        if not tokens:
            return None, None

        left, err = self.parse_operand(tokens)
        if err: return None, err
        if not left: return None, None

        op_token = tokens.current
        if not op_token or op_token.tag != 'operator':
            return left, None

        token = tokens.move_next().current
        if not token:
            return left, None

        right, err = self.parse_operand(tokens)
        if err: return None, err
        if not right: return None, error.InvalidSyntaxError([token])

        value_stack = [left, right]
        op_stack = [op_token.value]
        
        while tokens:
            op_token = tokens.current
            if not op_token or op_token.tag != 'operator':
                break

            op = op_token.value
            rank = self.OPERATOR_RANK[op]
            while rank < self.OPERATOR_RANK[op_stack[-1]]:
                right = value_stack.pop()
                left = value_stack.pop()
                value_stack.append((op_stack.pop(), left, right))
            
            op_stack.append(op)

            right, err = self.parse_operand(tokens.move_next())
            if err: return None, err
            if not right: return None, error.InvalidSyntaxError([token])
            value_stack.append(right)

        right = value_stack.pop()
        left = value_stack.pop()
        expr = (op_stack.pop(), left, right)
        while op_stack:
            op = op_stack.pop()
            if op != expr[0] or op == '^':
                expr = (op, value_stack.pop(), expr)
            else:
                expr = (expr[0], (op, value_stack.pop(), expr[1]), expr[2])

        return expr, None

    def format(self):
        '''Returns a formatted string representation of the AST.
        '''
        result = []
        
        #def _fmt(node, ind=''):
        #    '''Recursively formats the AST.'''
        #    if node is None:
        #        return
        #    elif isinstance(node, list):
        #        for i in node:
        #            _fmt(i, ind)
        #        return
        #    elif node.category in set(('expr', 'block')):
        #        result.append(ind + '<' + (str(node)))
        #        _fmt(node.expr, ind+'  ')
        #        result.append(ind + '>' + (str(node.close) if node.close else "?"))
        #    else:
        #        result.append(ind + str(node).strip())
        #    _fmt(node.left, ind+'  ')
        #    _fmt(node.right, ind+'  ')
        #
        #_fmt(self.expr)
        
        return '\n'.join(result)

    def __str__(self):
        '''Returns an abbreivated string representation of the AST.
        '''
        result = []

        #def _fmt(node):
        #    '''Recursively formats the AST.'''
        #    if node is None:
        #        return
        #    elif isinstance(node, list):
        #        for i in node:
        #            _fmt(i)
        #            result.append(';')
        #        if node: result.pop()
        #        return
        #    elif node.category == 'expr':
        #        result.extend(('<{', node.tag, '}'))
        #        _fmt(node.expr)
        #        result.extend(('{', node.close.tag if node.close else '?', '}>'))
        #    elif node.tag == 'BEGIN':
        #        result.extend(('<{', node.tag, ':', str(node.data or ''), '};'))
        #        _fmt(node.expr)
        #        result.extend((';{', node.close.tag if node.close else '?', '}>'))
        #    elif node.tag == 'REPEAT':
        #        result.extend(('<{', node.tag, ':'))
        #        _fmt(node.data)
        #        result.append('};')
        #        _fmt(node.expr)
        #        result.extend((';{', node.close.tag if node.close else '?', '}>'))
        #    else:
        #        result.append(node.tag)
        #    if node.left or node.right:
        #        result.append('{')
        #        _fmt(node.left)
        #        result.append(',')
        #        _fmt(node.right)
        #        result.append('}')
        #        
        #_fmt(self.expr)
        
        return ''.join(result)


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
