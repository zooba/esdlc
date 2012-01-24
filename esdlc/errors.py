'''This module contains all errors and warnings that may be raised while
compiling ESDL code.
'''

class ESDLSyntaxErrorBase(BaseException):
    '''Returned when an error or warning is encountered while compiling
    ESDL code.
    
    Derived classes should specify their own `code` and
    `default_message`. Initialisers may also be overridden.
    
    Warnings should also derive from this class, but use a code
    beginning with 'W'. All codes should be unique. Codes with different
    prefixes (that is, either 'W' or 'E') are considered to be
    different.
    
    Errors are ordered by line number, code and then message.
    '''
    code = "E0000"
    default_message = "Unspecified error"
    
    def __init__(self, tokens, *args, **kwargs):
        super(ESDLSyntaxErrorBase, self).__init__()
        self.message = kwargs.get('message', self.default_message)
        if ('%' in self.message) and args:
            try: self.message = self.message % args
            except TypeError: pass
        if tokens is None:
            self.line, self.col, self.text, self.length = None, None, None, None
        elif isinstance(tokens, list):
            tokens = sorted(tokens)
            self.line, self.col = tokens[0].line, tokens[0].col
            self.length = tokens[-1].col + len(tokens[-1].value) - tokens[0].col
            self.text = ''.join(token.value for token in tokens)
        else:
            self.line, self.col = tokens.line, tokens.col
            self.text, self.length = tokens.value, len(tokens.value)
        self.as_tuple = (self.line, self.col, self.code, self.message)
    
    @property
    def iswarning(self):
        '''Returns `True` if this error is a warning.
        
        Warnings are identified by a code beginning with ``W``.'''
        return self.code[0] == 'W'
    
    def __str__(self):
        if self.line is not None and self.col is not None:
            return '[%s] %s (ESDL Definition, line %d, char %d)' % (self.code, self.message, self.line+1, self.col+1)
        else:
            return '[%s] %s (ESDL Definition)' % (self.code, self.message)
    
    def __hash__(self):
        return hash(self.as_tuple)

    def __eq__(self, other):
        try:
            return self.as_tuple == other.as_tuple
        except AttributeError:
            return NotImplemented
    
    def __ne__(self, other):
        try:
            return self.as_tuple != other.as_tuple
        except AttributeError:
            return NotImplemented
    
    def __gt__(self, other):
        try:
            return self.as_tuple > other.as_tuple
        except AttributeError:
            return NotImplemented
    
    def __ge__(self, other):
        try:
            return self.as_tuple >= other.as_tuple
        except AttributeError:
            return NotImplemented
    
    def __lt__(self, other):
        try:
            return self.as_tuple < other.as_tuple
        except AttributeError:
            return NotImplemented
    
    def __le__(self, other):
        try:
            return self.as_tuple <= other.as_tuple
        except AttributeError:
            return NotImplemented
    

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Error/warnings used by AST
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class InvalidSyntaxError(ESDLSyntaxErrorBase):
    '''Returned when invalid syntax is encountered.'''
    code = "E0001"
    default_message = "Invalid syntax"

class UnexpectedEndOfDefinitionError(ESDLSyntaxErrorBase):
    '''Returned when the end of the definition is reached unexpectedly.
    '''
    code = "E0002"
    default_message = "Unexpected end of definition"

class UnexpectedCommandError(ESDLSyntaxErrorBase):
    '''Returned when a command appears outside of any block.'''
    code = "E0003"
    default_message = "Command not permitted outside of block"

class ExpectedBlockNameError(ESDLSyntaxErrorBase):
    '''Returned when a BEGIN command omits the block name.'''
    code = "E0004"
    default_message = "Block name expected"

class UnexpectedBlockNestingError(ESDLSyntaxErrorBase):
    '''Returned when a BEGIN command appears within another block.'''
    code = "E0005"
    default_message = "Blocks cannot be nested"

class UnmatchedEndError(ESDLSyntaxErrorBase):
    '''Returned when an END command has no matching BEGIN or REPEAT.'''
    code = "E0006"
    default_message = "END without a matching BEGIN or REPEAT"

class UnexpectedStatementError(ESDLSyntaxErrorBase):
    '''Returned when a statement appears in an odd location.'''
    code = "E0007"
    default_message = "Statement cannot be specified outside of a block"

class ExpectedRepeatCountError(ESDLSyntaxErrorBase):
    '''Returned when a REPEAT command omits the repeat count.'''
    code = "E0008"
    default_message = "Repeat count expected"

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Errors/warnings used by AST nodes
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class InvalidFunctionCallError(ESDLSyntaxErrorBase):
    '''Returned when a function call is not valid.'''
    code = "E1001"
    default_message = "Invalid function call"

class InvalidGroupError(ESDLSyntaxErrorBase):
    '''Returned when a group specification is not valid.'''
    code = "E1002"
    default_message = "Invalid group specification"

class ExpectedSelectError(ESDLSyntaxErrorBase):
    '''Returned when a SELECT command was expected but not found.'''
    code = "E1003"
    default_message = "Expected SELECT"

class ExpectedUsingError(ESDLSyntaxErrorBase):
    '''Returned when a USING command was expected but not found.'''
    code = "E1004"
    default_message = "Expected USING"

class ExpectedIntoError(ESDLSyntaxErrorBase):
    '''Returned when an INTO command was expected but not found.'''
    code = "E1005"
    default_message = "Expected INTO"

class ExpectedEvalError(ESDLSyntaxErrorBase):
    '''Returned when an EVAL(UATE) command was expected but not found.
    '''
    code = "E1006"
    default_message = "Expected EVAL or EVALUATE"

class ExpectedGroupError(ESDLSyntaxErrorBase):
    '''Returns when a group specification was expected but not found.'''
    code = "E1007"
    default_message = "Expected group or groups"

class ExpectedEvaluatorError(ESDLSyntaxErrorBase):
    '''Returned when an evaluator was expected but not found.'''
    code = "E1008"
    default_message = "Expected evaluator"

class GeneratorAsDestinationError(ESDLSyntaxErrorBase):
    '''Returned when a generator or function is the destination of a
    FROM statement.
    '''
    code = "E1009"
    default_message = "Generator cannot be specified as a destination group"

class InvalidVariableError(ESDLSyntaxErrorBase):
    '''Returned when a variable name or attribute is invalid.'''
    code = "E1010"
    default_message = "'%s' is not a valid variable name"

class ExpectedOperatorError(ESDLSyntaxErrorBase):
    '''Returned when an operator was expected but not found.'''
    code = "E1011"
    default_message = "Expected operator"

class InvalidNumberError(ESDLSyntaxErrorBase):
    '''Returned when a number turns out to not really be a number.'''
    code = "E1012"
    default_message = "'%s' is not a valid number"

class InvalidAssignmentError(ESDLSyntaxErrorBase):
    '''Returned when an assignment destination is invalid.'''
    code = "E1010"
    default_message = "Invalid assignment destination"

class InvalidParameterNameError(ESDLSyntaxErrorBase):
    '''Returned when a parameter name is invalid.'''
    code = "E1011"
    default_message = "Invalid parameter name"

class ExpectedParameterValueError(ESDLSyntaxErrorBase):
    '''Returned when a parameter=value pair is not found.'''
    code = "E1012"
    default_message = "Expected parameter=value pair"

class UnmatchedBracketError(ESDLSyntaxErrorBase):
    '''Returned when brackets or parentheses are unmatched.'''
    code = "E1013"
    default_message = "Closing for '%s' not found"

class ExpectedCommaError(ESDLSyntaxErrorBase):
    '''Returned when a comma separator is not found.'''
    code = "E1014"
    default_message = "Expected comma separator"

class ExpectedIndexError(ESDLSyntaxErrorBase):
    '''Returned when a subscript does not contain an index/key.'''
    code = "E1015"
    default_message = "Expected index in subscript"

class RepeatedParameterNameError(ESDLSyntaxErrorBase):
    '''Returned when a parameter name is repeated.'''
    code = "E1016"
    default_message = "Parameter '%s' is specified multiple times"

class ExpectedJoinerError(ESDLSyntaxErrorBase):
    '''Returned when a joiner was expected but not found.'''
    code = "E1017"
    default_message = "Expected joiner"


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Errors/warnings used by Verifier
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class AmbiguousVariableBlockNameError(ESDLSyntaxErrorBase):
    '''Returned when a block name is also used as a variable name.'''
    code = "E2001"
    default_message = "Block name '%s' is also used as a variable"

class RepeatedDestinationGroupError(ESDLSyntaxErrorBase):
    '''Returned when a SELECT clause specifies the same group multiple
    times.
    '''
    code = "E2002"
    default_message = "Group '%s' is selected into multiple times"

class InvalidGroupSizeError(ESDLSyntaxErrorBase):
    '''Returned when the size specifier for a group is not an acceptable
    type.
    '''
    code = "E2003"
    default_message = "Size specifier for group '%s' is not valid"

class UnexpectedGroupSizeError(ESDLSyntaxErrorBase):
    '''Returned when a size specifier for a group is not allowed.'''
    code = "E2004"
    default_message = "Size specifier for group '%s' not permitted"

class AmbiguousVariableGroupNameError(ESDLSyntaxErrorBase):
    '''Returned when a group name is also used as a variable name.'''
    code = "E2005"
    default_message = "Group name '%s' is also used as a variable"

class AmbiguousGroupGeneratorNameError(ESDLSyntaxErrorBase):
    '''Returned when a group name is also used as a generator name.'''
    code = "E2006"
    default_message = "Group name '%s' is also used as a generator"

class RepeatedGroupError(ESDLSyntaxErrorBase):
    '''Returned when a YIELD or EVAL clause specifies the same group
    multiple times.
    '''
    code = "W2002"
    default_message = "Group '%s' is unnecessarily specified more than once"

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class InternalVariableNameError(ESDLSyntaxErrorBase):
    '''Returned when a variable name begins with an underscore.'''
    code = "W2001"
    default_message = "Variable '%s' may be overwritten by the compiler"

class UninitialisedGlobalError(ESDLSyntaxErrorBase):
    '''Returned when a global variable is not initialised within the
    definition.
    '''
    code = "W2002"
    default_message = "Global variable '%s' not initialised"

class UninitialisedConstantError(ESDLSyntaxErrorBase):
    '''Returned when a global constant variable is not initialised
    within the definition.
    '''
    code = "W2003"
    default_message = "Constant '%s' not initialised"

class UninitialisedVariableError(ESDLSyntaxErrorBase):
    '''Returned when a variable is not initialised within the
    definition.
    '''
    code = "W2004"
    default_message = "Variable '%s' not initialised"

class UnusedVariableError(ESDLSyntaxErrorBase):
    '''Returned when a variable is initialised but not used within the
    definition.
    '''
    code = "W2005"
    default_message = "Variable '%s' unused"

class InaccessibleGroupError(ESDLSyntaxErrorBase):
    '''Returned when a group appears after an unbounded group and will
    never be selected into.
    '''
    code = "W2006"
    default_message = "Group '%s' specified after an unbounded group"

class UninitialisedGroupError(ESDLSyntaxErrorBase):
    '''Returned when a group is not initialised within the definition.
    '''
    code = "W2007"
    default_message = "Group '%s' not initialised"

class UnusedGroupError(ESDLSyntaxErrorBase):
    '''Returned when a group is initialised but not used within the
    definition.
    '''
    code = "W2008"
    default_message = "Group '%s' unused"

class InternalParameterNameError(ESDLSyntaxErrorBase):
    '''Returned when a parameter name begins with an underscore.'''
    code = "W2009"
    default_message = "Parameter '%s' may be used internally by the compiler"

