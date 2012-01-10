import itertools
import esdlc.errors as error
import esdlc.ast
import esdlc.ast.lexer

def check(source, expected):
    tokens = esdlc.ast.lexer.TokenReader(esdlc.ast.lexer.tokenise(source))
    ast = esdlc.ast.AST(tokens)
    actual = str(ast)
    try:
        success = False
        assert expected == actual, "Expected %s\nActual   %s" % (expected, actual)
        success = True
    finally:
        if not success:
            print ast.format()

def check_error(source, expected_error_types):
    tokens = esdlc.ast.lexer.TokenReader(esdlc.ast.lexer.tokenise(source))
    ast = esdlc.ast.AST(tokens)
    try:
        success = False
        for expected, actual in itertools.izip_longest(expected_error_types, ast._errors):
            assert expected is not None, "Expected None, Actual = %s" % (actual)
            assert actual is not None, "Expected %s, Actual = None" % (expected)
            assert isinstance(actual, expected), "Expected %s, Actual = %s" % (expected, actual)
        success = True
    finally:
        if not success:
            print 'Actual: %s' % ast
            print 'Errors:\n  ' + '\n  '.join(str(i) for i in ast.errors)
            print 'Warnings:\n  ' + '\n  '.join(str(i) for i in ast.warnings)

def test_errors():
    for op in "+-*/^%":
        yield check_error, "A=A" + op, [error.InvalidSyntaxError]
    for op in "*/^%":
        yield check_error, "A=" + op + "A", [error.InvalidSyntaxError]
    
    yield check_error, "A=,A", [error.InvalidSyntaxError]
    yield check_error, "A=A,", [error.InvalidSyntaxError]
    yield check_error, "A=A,,", [error.InvalidSyntaxError]
    yield check_error, "A=A,,,", [error.InvalidSyntaxError]
    yield check_error, "A=,,A,,", [error.InvalidSyntaxError]
    yield check_error, "A=A, B, ", [error.InvalidSyntaxError]
    yield check_error, "A=(A, B, )", [error.UnmatchedBracketError]
    yield check_error, "A=(A, ,B)", [error.UnmatchedBracketError]

def test_arithmetic_without_parens():
    for op in "+-*/^%":
        yield check, "A=1%s2" % op, "={a,%s{1.0,2.0}}" % op

    for op in "+-*/%":
        p = (op, op)
        yield check, "A=1%s2%s3" % p, "={a,%s{%s{1.0,2.0},3.0}}" % p
    yield check, "A=1^2^3", "={a,^{1.0,^{2.0,3.0}}}"

    for op in "+-*/^%":
        yield check, "A=1%s+2" % op, "={a,%s{1.0,+{,2.0}}}" % op
    for op in "+-*/^%":
        yield check, "A=1%s-2" % op, "={a,%s{1.0,-{,2.0}}}" % op
    
    yield check, "A=1+2-3", "={a,-{+{1.0,2.0},3.0}}"
    yield check, "A=1-2+3", "={a,+{-{1.0,2.0},3.0}}"
    for higher_op in "*/^%":
        for lower_op in "+-":
            p1 = (lower_op, higher_op)
            p2 = (higher_op, lower_op)
            yield check, "A=1%s2%s3" % p1, "={a,%s{1.0,%s{2.0,3.0}}}" % p1
            yield check, "A=1%s2%s3" % p2, "={a,%s{%s{1.0,2.0},3.0}}" % p1 # p1, not p2

def test_arithmetic_with_parens():
    for op in "+-*/^%":
        yield check, "A=((1)%s(2))" % op, "={a,%s{1.0,2.0}}" % op

    for op in "+-*/%":
        p = (op, op)
        yield check, "A=1%s(2%s3)" % p, "={a,%s{1.0,%s{2.0,3.0}}}" % p
    yield check, "A=(1^2)^3", "={a,^{^{1.0,2.0},3.0}}"

    for op in "+-*/^%":
        yield check, "A=1%s(+2)" % op, "={a,%s{1.0,+{,2.0}}}" % op
    for op in "+-*/^%":
        yield check, "A=1%s(-2)" % op, "={a,%s{1.0,-{,2.0}}}" % op
    
    yield check, "A=1+(2-3)", "={a,+{1.0,-{2.0,3.0}}}"
    yield check, "A=1-(2+3)", "={a,-{1.0,+{2.0,3.0}}}"
    for higher_op in "*/^%":
        for lower_op in "+-":
            p1 = (lower_op, higher_op)
            p2 = (higher_op, lower_op)
            yield check, "A=(1%s2)%s3" % p1, "={a,%s{%s{1.0,2.0},3.0}}" % p2   # p2, not p1
            yield check, "A=1%s(2%s3)" % p2, "={a,%s{1.0,%s{2.0,3.0}}}" % p2

def test_arithmetic_with_variables():
    for op in "+-*/^%":
        yield check, "A=A%sB" % op, "={a,%s{a,b}}" % op

    for op in "+-*/%":
        p = (op, op)
        yield check, "A=A%sB%sC" % p, "={a,%s{%s{a,b},c}}" % p
    yield check, "A=A^B^C", "={a,^{a,^{b,c}}}"

    for op in "+-*/^%":
        yield check, "A=A%s+B" % op, "={a,%s{a,+{,b}}}" % op
    for op in "+-*/^%":
        yield check, "A=A%s-B" % op, "={a,%s{a,-{,b}}}" % op
    
    yield check, "A=A+B-C", "={a,-{+{a,b},c}}"
    yield check, "A=A-B+C", "={a,+{-{a,b},c}}"
    for higher_op in "*/^%":
        for lower_op in "+-":
            p1 = (lower_op, higher_op)
            p2 = (higher_op, lower_op)
            yield check, "A=A%sB%sC" % p1, "={a,%s{a,%s{b,c}}}" % p1
            yield check, "A=A%sB%sC" % p2, "={a,%s{%s{a,b},c}}" % p1 # p1, not p2

def test_dotted_names():
    yield check, "A=B.C", "={a,.{b,c}}"
    yield check, "A=B.C.D", "={a,.{.{b,c},d}}"
    yield check, "A=B.C.D.E", "={a,.{.{.{b,c},d},e}}"
    
    for op in "+-*/^%":
        yield check, "A = B.C %s D.E" % op, "={a,%s{.{b,c},.{d,e}}}" % op
    for op in "+-*/^%":
        yield check, "A = -B.C %s -D.E" % op, "={a,%s{-{,.{b,c}},-{,.{d,e}}}}" % op
    
    yield check, "A = B.(C+D).E", "={a,.{.{b,+{c,d}},e}}"

    for open, close, tag in [('(', ')', 'CallFunc'), ('[', ']', 'GetElement')]:
        p1 = (open, close)
        p2 = p1 * 2
        p3 = p1 * 3
        r1 = (tag, )
        r2 = r1 * 2
        r3 = r1 * 3
        yield check, "A.B%s%s" % p1, "%s{.{a,b},}" % r1
        yield check, "A.B.C%s%s" % p1, "%s{.{.{a,b},c},}" % r1
        yield check, "A.B%s%s.C" % p1, ".{%s{.{a,b},},c}" % r1
        yield check, "A%s%s.B.C" % p1, ".{.{%s{a,},b},c}" % r1
        yield check, "A.B%s%s.C%s%s" % p2, "%s{.{%s{.{a,b},},c},}" % r2
        yield check, "A%s%s.B.C%s%s" % p2, "%s{.{.{%s{a,},b},c},}" % r2
        yield check, "A%s%s.B%s%s.C%s%s" % p3, "%s{.{%s{.{%s{a,},b},},c},}" % r3


def test_function_call():
    yield check, "A[a]", "GetElement{a,a}"

    yield check, "A()", "CallFunc{a,}"
    yield check, "A(a)", "CallFunc{a,[a{}]}"
    yield check, "A(a,b)", "CallFunc{a,[a{},b{}]}"
    yield check, "A(a,b,c)", "CallFunc{a,[a{},b{},c{}]}"
    yield check, "A(a,b,c,d)", "CallFunc{a,[a{},b{},c{},d{}]}"
    yield check, "A(a=1)", "CallFunc{a,[a{1.0}]}"
    yield check, "A(a=1,b=2)", "CallFunc{a,[a{1.0},b{2.0}]}"
    yield check, "A(a=1,b=2,c=3)", "CallFunc{a,[a{1.0},b{2.0},c{3.0}]}"
    yield check, "A(a=1,b=2,c=3,d=4)", "CallFunc{a,[a{1.0},b{2.0},c{3.0},d{4.0}]}"

    yield check, "A(a=-1,b=2+3,c=3*4-5,d=4/5)", "CallFunc{a,[a{-{,1.0}},b{+{2.0,3.0}},c{-{*{3.0,4.0},5.0}},d{/{4.0,5.0}}]}"
    yield check, "A(a=-t,b=u+v,c=w*(x-y),d=y/z)", "CallFunc{a,[a{-{,t}},b{+{u,v}},c{*{w,-{x,y}}},d{/{y,z}}]}"

    yield check, "A(b=B(C))", "CallFunc{a,[b{CallFunc{b,[c{}]}}]}"
    yield check, "A(b=B(c=C))", "CallFunc{a,[b{CallFunc{b,[c{c}]}}]}"

    yield check, "A(b=B[C])", "CallFunc{a,[b{GetElement{b,c}}]}"
    yield check, "A[B(c=C)]", "GetElement{a,CallFunc{b,[c{c}]}}"


def test_pragma():
    yield check, "`Any Text Here", "PragmaStmt{`Any Text Here}"

def test_comments():
    yield check, "A = B + C # comment", "={a,+{b,c}}"
    yield check, "A = B + C ; comment", "={a,+{b,c}}"
    yield check, "A = B + C // comment", "={a,+{b,c}}"

def test_literals():
    test_values = [
        ('True', ['Tt', 'Rr', 'Uu', 'Ee']),
        ('False', ['Ff', 'Aa', 'Ll', 'Ss', 'Ee']),
        ('None', ['Nn', 'Oo', 'Nn', 'Ee']),
        ('None', ['Nn', 'Uu', 'Ll', 'Ll'])
    ]

    for expected, source in test_values:
        for s in (''.join(i) for i in itertools.product(*source)):
            yield check, "A = %s" % s, "={a,Constant{%s}}" % expected

def test_continuation():
    for cont in ['\\\n', '\\ \n', '\\\t\n', '\\#comment\n', '\\ #comment\n', '\\\t#comment\n']:
        yield check, "A %s = B + C" % cont, "={a,+{b,c}}"
        yield check, "A = %s B + C" % cont, "={a,+{b,c}}"
        yield check, "A = B %s + C" % cont, "={a,+{b,c}}"
        yield check, "A = B + %s C" % cont, "={a,+{b,c}}"
        yield check, "A = B + C %s" % cont, "={a,+{b,c}}"

def test_fromselect_stmt():
    raise StopIteration
    yield check, "FROM group SELECT group", "SELECT{FROM{,group},group}"
    yield check, "FROM size group SELECT group", "SELECT{FROM{,size{,group}},group}"
    yield check, "FROM (size) group SELECT group", "SELECT{FROM{,<{(}size{)}>{,group}},group}"
    yield check, "FROM 10 group SELECT group", "SELECT{FROM{,10.0{,group}},group}"
    yield check, "FROM (10) group SELECT group", "SELECT{FROM{,<{(}10.0{)}>{,group}},group}"
    yield check, "FROM gen() SELECT group", "SELECT{FROM{,gen{,<{(}{)}>}},group}"
    
    yield check, "FROM group SELECT size group", "SELECT{FROM{,group},size{,group}}"
    yield check, "FROM group SELECT (size) group", "SELECT{FROM{,group},<{(}size{)}>{,group}}"
    yield check, "FROM group SELECT 10 group", "SELECT{FROM{,group},10.0{,group}}"
    yield check, "FROM group SELECT (10) group", "SELECT{FROM{,group},<{(}10.0{)}>{,group}}"
    yield check, "FROM group SELECT gen()", "SELECT{FROM{,group},gen{,<{(}{)}>}}"

    yield check, "FROM group1, group2 SELECT group", "SELECT{FROM{,,{group1,group2}},group}"
    yield check, "FROM group1, group2, group3 SELECT group", "SELECT{FROM{,,{,{group1,group2},group3}},group}"
    yield check, "FROM group1, (size) group2 SELECT group", "SELECT{FROM{,,{group1,<{(}size{)}>{,group2}}},group}"
    yield check, "FROM group SELECT group1, group2", "SELECT{FROM{,group},,{group1,group2}}"
    yield check, "FROM group SELECT group1, group2, group3", "SELECT{FROM{,group},,{,{group1,group2},group3}}"
    yield check, "FROM group SELECT group1, (size) group2", "SELECT{FROM{,group},,{group1,<{(}size{)}>{,group2}}}"

    yield check, "FROM group SELECT group USING op", "USING{SELECT{FROM{,group},group},op}"
    yield check, "FROM group SELECT group USING op1, op2", "USING{SELECT{FROM{,group},group},,{op1,op2}}"
    yield check, "FROM group SELECT group USING op1, op2, op3", "USING{SELECT{FROM{,group},group},,{,{op1,op2},op3}}"
    yield check, "FROM group SELECT group USING op(a=A,b=B)", "USING{SELECT{FROM{,group},group},op{,<{(},{={a,A},={b,B}}{)}>}}"

def test_joininto_stmt():
    raise StopIteration
    yield check, "JOIN group INTO group", "INTO{JOIN{,group},group}"
    yield check, "JOIN size group INTO group", "INTO{JOIN{,size{,group}},group}"
    yield check, "JOIN (size) group INTO group", "INTO{JOIN{,<{(}size{)}>{,group}},group}"
    yield check, "JOIN 10 group INTO group", "INTO{JOIN{,10.0{,group}},group}"
    yield check, "JOIN (10) group INTO group", "INTO{JOIN{,<{(}10.0{)}>{,group}},group}"
    yield check, "JOIN gen() INTO group", "INTO{JOIN{,gen{,<{(}{)}>}},group}"
    
    yield check, "JOIN group INTO size group", "INTO{JOIN{,group},size{,group}}"
    yield check, "JOIN group INTO (size) group", "INTO{JOIN{,group},<{(}size{)}>{,group}}"
    yield check, "JOIN group INTO 10 group", "INTO{JOIN{,group},10.0{,group}}"
    yield check, "JOIN group INTO (10) group", "INTO{JOIN{,group},<{(}10.0{)}>{,group}}"
    yield check, "JOIN group INTO gen()", "INTO{JOIN{,group},gen{,<{(}{)}>}}"

    yield check, "JOIN group1, group2 INTO group", "INTO{JOIN{,,{group1,group2}},group}"
    yield check, "JOIN group1, group2, group3 INTO group", "INTO{JOIN{,,{,{group1,group2},group3}},group}"
    yield check, "JOIN group1, (size) group2 INTO group", "INTO{JOIN{,,{group1,<{(}size{)}>{,group2}}},group}"
    yield check, "JOIN group INTO group1, group2", "INTO{JOIN{,group},,{group1,group2}}"
    yield check, "JOIN group INTO group1, group2, group3", "INTO{JOIN{,group},,{,{group1,group2},group3}}"
    yield check, "JOIN group INTO group1, (size) group2", "INTO{JOIN{,group},,{group1,<{(}size{)}>{,group2}}}"

    yield check, "JOIN group INTO group USING op", "USING{INTO{JOIN{,group},group},op}"
    yield check, "JOIN group INTO group USING op1, op2", "USING{INTO{JOIN{,group},group},,{op1,op2}}"
    yield check, "JOIN group INTO group USING op1, op2, op3", "USING{INTO{JOIN{,group},group},,{,{op1,op2},op3}}"
    yield check, "JOIN group INTO group USING op(a=A,b=B)", "USING{INTO{JOIN{,group},group},op{,<{(},{={a,A},={b,B}}{)}>}}"

def test_eval_stmt():
    raise StopIteration
    for s in ["EVAL", "EVALUATE"]:
        yield check, s + " group", "EVAL{,group}"
        yield check, s + " group USING evaluator", "USING{EVAL{,group},evaluator}"
        yield check, s + " group USING evaluator(t)", "USING{EVAL{,group},evaluator{,<{(}t{)}>}}"
        yield check, s + " group USING evaluator1, evaluator2", "USING{EVAL{,group},,{evaluator1,evaluator2}}"
    
        yield check, s + " group1, group2", "EVAL{,,{group1,group2}}"
        yield check, s + " group1, group2 USING evaluator", "USING{EVAL{,,{group1,group2}},evaluator}"
        yield check, s + " group1, group2 USING evaluator(t)", "USING{EVAL{,,{group1,group2}},evaluator{,<{(}t{)}>}}"
        yield check, s + " group1, group2 USING evaluator1, evaluator2", "USING{EVAL{,,{group1,group2}},,{evaluator1,evaluator2}}"

def test_yield_stmt():
    yield check, "YIELD group", "YieldStmt{[group]}"
    yield check, "YIELD group1, group2", "YieldStmt{[group1,group2]}"

def test_begin_stmt():
    yield check, "BEGIN NAME\nEND", "BeginStmt{name};EndStmt{}"
    yield check, "BEGIN NAME anything\nEND anything", "BeginStmt{name};EndStmt{}"

def test_repeat_stmt():
    yield check, "REPEAT 10\nEND anything", "RepeatStmt{10.0};EndStmt{}"
    yield check, "REPEAT A\nEND anything", "RepeatStmt{a};EndStmt{}"
    yield check, "REPEAT A+B+C\nEND anything", "RepeatStmt{+{+{a,b},c}};EndStmt{}"
    yield check, "REPEAT A*B+C(D=E)\nEND anything", "RepeatStmt{+{*{a,b},CallFunc{c,[d{e}]}}};EndStmt{}"

HUGE_TEST = r'''FROM random_binary(length=config.length.max) SELECT 100 population
t = 0
delta_t = -0.1
EVAL population USING evaluators.population(t)
YIELD population

BEGIN generation
    t = t + (delta_t * 1.4)
    `print t

    REPEAT 10
        FROM population SELECT 100 parents \
            USING tournament(k=2, greediness=0.7)
        
        mutate_parents = parents
        parents2 = parents
        crossover_parents = parents
        FROM mutate_parents SELECT mutated USING mutate_delta(stepsize)
        FROM crossover_parents SELECT crossed USING uniform_crossover
        EVAL mutated, crossed USING evaluator(t=t)
        
        JOIN mutated, crossed INTO merged USING tuples
        FROM merged SELECT offspring USING best_of_tuple
        
        FROM population, offspring SELECT 99 population, rest USING best
        FROM rest SELECT 1 extras USING uniform_random
        FROM population, rest, extras SELECT (((100))) population
    END REPEAT
    
    EVAL population USING evaluators.config(t)
    YIELD population
END generation'''

HUGE_TEST_EXPECTED = '''SELECT{FROM{,random_binary{,<{(}={length,.{.{config,length},max}}{)}>}},100.0{,population}}
={t,0.0}
={delta_t,-{,0.1}}
USING{EVAL{,population},.{evaluators,population{,<{(}t{)}>}}}
YIELD{,population}

<{BEGIN:generation}
={t,+{t,<{(}*{delta_t,1.4}{)}>}}
`print t
<{REPEAT:10.0}
USING{SELECT{FROM{,population},100.0{,parents}},tournament{,<{(},{={k,2.0},={greediness,0.7}}{)}>}}
={mutate_parents,parents}
={parents2,parents}
={crossover_parents,parents}
USING{SELECT{FROM{,mutate_parents},mutated},mutate_delta{,<{(}stepsize{)}>}}
USING{SELECT{FROM{,crossover_parents},crossed},uniform_crossover}
USING{EVAL{,,{mutated,crossed}},evaluator{,<{(}={t,t}{)}>}}
USING{INTO{JOIN{,,{mutated,crossed}},merged},tuples}
USING{SELECT{FROM{,merged},offspring},best_of_tuple}
USING{SELECT{FROM{,,{population,offspring}},,{99.0{,population},rest}},best}
USING{SELECT{FROM{,rest},1.0{,extras}},uniform_random}
SELECT{FROM{,,{,{population,rest},extras}},<{(}<{(}<{(}100.0{)}>{)}>{)}>{,population}}
{END}>
USING{EVAL{,population},.{evaluators,config{,<{(}t{)}>}}}
YIELD{,population}
{END}>'''

def test_multiple():
    yield check, "A = B + C\nD = func(a=A)\nA = A + 1", "={a,+{b,c}};={d,CallFunc{func,[a{a}]}};={a,+{a,1.0}}"
    raise StopIteration
    yield check, "FROM a SELECT b USING x\nFROM b, c SELECT d USING y\nz = m(A=a, C=c)", \
        "USING{SELECT{FROM{,a},b},x};USING{SELECT{FROM{,,{b,c}},d},y};={z,m{,<{(},{={A,a},={C,c}}{)}>}}"
    
    yield check, HUGE_TEST, HUGE_TEST_EXPECTED.replace('\n', ';')