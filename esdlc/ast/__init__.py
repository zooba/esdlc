'''Provides the `AST` object for parsing a syntax tree from source
code.
'''

__all__ = ['parse', 'load']

import itertools
import esdlc.errors as error
from esdlc.ast.nodes import Node
from esdlc.ast.validator import Validator
from esdlc.ast.lexer import TokenReader

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
                tokens.push_tokenset()
                while tokens and tokens.current.tag != 'eos': tokens.move_next()
                return Node('EndStmt', tokens=tokens.pop_tokenset())
            elif peek and peek.tag == 'assign':
                return self.parse_assign_stmt(tokens)
            else:
                return self.parse_expression(tokens)
        elif token.tag == 'pragma':
            tokens.push_tokenset()
            while tokens and tokens.current.tag != 'eos': tokens.move_next()
            return Node('PragmaStmt', token.value, tokens=tokens.pop_tokenset())
        elif token.tag == 'comment':
            return Node('Comment', token.value, tokens=[token])
        elif token.tag == 'eos':
            return None
        else:
            raise error.InvalidSyntaxError([token])
                
    def parse_from_stmt(self, tokens):
        tokens.push_tokenset()
        sources = self.parse_groups_or_generators(tokens.move_next())
        
        if not tokens.current or tokens.current.value != 'select':
            tokens.drop_tokenset()
            raise error.ExpectedSelectError([tokens.current])
        
        destinations = self.parse_sized_groups(tokens.move_next())
        operators = self.parse_using(tokens)

        return Node('FromStmt', sources, destinations, operators, tokens=tokens.pop_tokenset())

    def parse_join_stmt(self, tokens):
        tokens.push_tokenset()
        sources = self.parse_groups_or_generators(tokens.move_next())
        
        if not tokens.current or tokens.current.value != 'into':
            tokens.drop_tokenset()
            raise error.ExpectedIntoError([tokens.current])
        
        destinations = self.parse_sized_groups(tokens.move_next())
        operators = self.parse_using(tokens)
        
        return Node('JoinStmt', sources, destinations, operators, tokens=tokens.pop_tokenset())

    def parse_yield_stmt(self, tokens):
        tokens.push_tokenset()
        sources = self.parse_groups(tokens.move_next())
        return Node('YieldStmt', sources, tokens=tokens.pop_tokenset())

    def parse_eval_stmt(self, tokens):
        tokens.push_tokenset()
        sources = self.parse_groups(tokens.move_next())
        operators = self.parse_using(tokens)
        return Node('EvalStmt', sources, operators, tokens=tokens.pop_tokenset())

    def parse_using(self, tokens):
        if not tokens.current or tokens.current.tag == 'eos':
            return []
        elif tokens.current and tokens.current.value == 'using':
            return self.parse_operators(tokens.move_next())
        else:
            raise error.ExpectedUsingError([tokens.current or tokens.last])

    def parse_begin_stmt(self, tokens):
        tokens.push_tokenset()
        stmt = tokens.current
        tokens.move_next()
        if not tokens or tokens.current.tag != 'name':
            tokens.drop_tokenset()
            raise error.ExpectedBlockNameError([stmt])
        name = tokens.current.value
        while tokens and tokens.current.tag != 'eos': tokens.move_next()
        return Node('BeginStmt', name, tokens=tokens.pop_tokenset())

    def parse_repeat_stmt(self, tokens):
        tokens.push_tokenset()
        tokens.move_next()
        if not tokens:
            tokens.drop_tokenset()
            raise error.ExpectedRepeatCountError([tokens.current])
        count = self.parse_expression(tokens)
        while tokens and tokens.current.tag != 'eos': tokens.move_next()
        return Node('RepeatStmt', count, tokens=tokens.pop_tokenset())

    def _parse_group_list(self, tokens, size_is_error = False, call_is_error = False):
        group_tokens = [[]]
        nesting = 0
        
        while tokens:
            token = tokens.current
            if token.tag == 'open':
                nesting += 1
                group_tokens[-1].append(token)
            elif token.tag == 'close':
                nesting -= 1
                group_tokens[-1].append(token)
            elif nesting == 0 and token.tag == 'comma':
                group_tokens.append([])
            elif nesting == 0 and token.tag == 'name' and token.value in frozenset(('select', 'into', 'using')):
                break
            elif token.tag == 'eos':
                break
            else:
                group_tokens[-1].append(token)
            tokens.move_next()

        groups = []
        any_errors = False
        for group_token_list in group_tokens:
            try:
                if group_token_list[-1].tag == 'name':
                    name = self.parse_operand(TokenReader(group_token_list[-1:]))
                    if not name or len(name) != 2 or name[0] != 'Name' or not name[1]:
                        raise error.InvalidGroupError(group_token_list)

                    if len(group_token_list) > 1:
                        if size_is_error:
                            raise error.UnexpectedGroupSizeError(group_token_list, name[1])

                        size = self.parse_expression(TokenReader(group_token_list[:-1]))
                    else:
                        size = None
                    groups.append(Node('Group', size, name, tokens=group_token_list))
                elif call_is_error:
                    raise error.GeneratorAsDestinationError(group_token_list)
                else:
                    expr = self.parse_expression(TokenReader(group_token_list))
                    groups.append(expr)
            except error.ESDLSyntaxErrorBase as err:
                self._errors.append(err)
                any_errors = True
                continue
            
        return groups if not any_errors else []

    def parse_groups(self, tokens):
        return self._parse_group_list(tokens, size_is_error=True, call_is_error=True)

    def parse_sized_groups(self, tokens):
        return self._parse_group_list(tokens, size_is_error=False, call_is_error=True)

    def parse_groups_or_generators(self, tokens):
        return self._parse_group_list(tokens, size_is_error=True, call_is_error=False)

    def parse_operators(self, tokens):
        operators = []
        for node in self._parse_group_list(tokens, size_is_error=True, call_is_error=False):
            if node.tag == 'Group' and node[1] is None:
                expr = Node('CallFunc', node[2], None, tokens=node.tokens)
            elif node.tag != 'CallFunc':
                expr = Node('CallFunc', node, None, tokens=node.tokens)
            else:
                expr = node
            operators.append(expr)
        return operators

    def parse_parameters(self, tokens):
        token = tokens.current
        peek = tokens.peek
        tokens.push_tokenset()

        if not token or token.tag == 'close':
            return Node('ParameterList', [])
        elif token.tag != 'name':
            raise error.InvalidParameterNameError([token])

        tokens.push_tokenset()
        parameters = []
        while token and token.tag == 'name':
            if peek.value == '=':
                arg = self.parse_expression(tokens.move_next().move_next())
                parameters.append(Node('Parameter', token.value, arg, tokens=tokens.pop_tokenset()))
            elif peek.value == ',':
                parameters.append(Node('Parameter', token.value, None, tokens=tokens.pop_tokenset()))
                tokens.move_next()
            elif peek.value == ')':
                parameters.append(Node('Parameter', token.value, None, tokens=tokens.pop_tokenset()))
            else:
                tokens.drop_tokenset()
                tokens.drop_tokenset()
                raise error.ExpectedParameterValueError([peek])
            
            if not tokens or tokens.current.value == ')':
                break
            
            token = tokens.move_next().current
            tokens.push_tokenset()
            peek = tokens.peek

        return Node('ParameterList', parameters, tokens=tokens.pop_tokenset())

    def parse_assign_stmt(self, tokens):
        tokens.push_tokenset()
        dest = self.parse_operand(tokens)
        if not tokens or tokens.current.value != '=':
            tokens.drop_tokenset()
            raise error.InvalidSyntaxError([tokens.current or dest])

        src = self.parse_expression(tokens.move_next())
        if not src:
            tokens.drop_tokenset()
            raise error.InvalidSyntaxError([tokens.current or dest])
        return Node('=', dest, src, tokens=tokens.pop_tokenset())

    CONSTANT_VALUES = {'true': True, 'false': False, 'null': None, 'none': None}

    def parse_operand(self, tokens):
        token = tokens.current
        peek = tokens.peek
        if not token:
            return None

        tokens.push_tokenset()
        if token.tag == 'name':
            name = tokens.current.value
            tokens.move_next()
            if name in self.CONSTANT_VALUES:
                return Node('Constant', self.CONSTANT_VALUES[name], tokens=tokens.pop_tokenset())
            else:
                return Node('Name', name, tokens=tokens.pop_tokenset())
        elif token.tag == 'number':
            tokens.move_next()
            try:
                node_tokens = tokens.pop_tokenset()
                return Node('Number', float(token.value), node_tokens)
            except:
                raise error.InvalidNumberError(node_tokens)
        elif token.tag == 'open':
            tokens.drop_tokenset()
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
                expr.append(Node('CallFunc', tokens=[tokens.current]))
                expr.append(self.parse_parameters(tokens.move_next()))
                if not tokens: raise error.InvalidFunctionCallError([op_token])
                if tokens.current.value != ')': raise error.InvalidFunctionCallError([tokens.current])
                tokens.move_next()
            elif op == '[':
                expr.append(Node('GetElement', tokens=[tokens.current]))
                expr.append(self.parse_expression(tokens.move_next()))
                if not tokens: raise error.UnmatchedBracketError([op_token], '[')
                if tokens.current.value != ']': raise error.UnmatchedBracketError([tokens.current], '[')
                tokens.move_next()
            else:
                expr.append(Node(op, tokens=[tokens.current]))
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
                if op.tag in match:
                    tokens = (left.tokens if left else []) + op.tokens + (right.tokens if right else [])
                    expr[i:i+3] = [Node(op.tag, left, right, tokens=tokens)]
                else:
                    i += 2

        def _reduce_unary(expr, match):
            i = 0
            while i + 3 <= len(expr):
                left, op, right = expr[i:i+3]
                if left is None and op.tag in match:
                    tokens=op.tokens + (right.tokens if right else [])
                    expr[i:i+3] = [Node(op.tag, None, right, tokens=tokens)]
                else:
                    i += 2
        
        def _reduce_reverse(expr, match):
            i = len(expr) - 3
            while i >= 0:
                left, op, right = expr[i:i+3]
                if op.tag in match:
                    tokens = (left.tokens if left else []) + op.tokens + (right.tokens if right else [])
                    expr[i:i+3] = [Node(op.tag, left, right, tokens=tokens)]
                i -= 2
        
        _reduce(expr, ('.', 'CallFunc', 'GetElement'))
        _reduce_unary(expr, '+-')
        _reduce(expr, '%')
        _reduce_reverse(expr, '^')
        _reduce(expr, '*/')
        _reduce(expr, '+-')
        

        assert len(expr) == 1
        assert isinstance(expr[0], Node)
        return expr[0]

    def __repr__(self):
        '''Returns a formatted string representation of the AST.
        '''
        return ';'.join(''.join(stmt.format([], raw=True)) for stmt in self.expr)

    def __str__(self):
        '''Returns an abbreivated string representation of the AST.
        '''
        return ';'.join(''.join(stmt.format([])) for stmt in self.expr)


def parse(source):
    '''Loads an `AST` instance from the contents of `source`.'''
    import esdlc.ast.lexer
    tokens = TokenReader(esdlc.ast.lexer.tokenise(source))
        
    self = AST(tokens)
    return self

def load(path):
    '''Loads an `AST` instance from the contents of the file at
    `path`.
    '''
    with open(path) as src:
        return parse(src)
