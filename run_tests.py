import itertools
import traceback
import tests.test_lexer
import tests.test_ast
import tests.test_emitters_esdl
import tests.test_model
import tests.test_model_validator
import tests.test_model_ast

total_count = 0
success_count = 0

all_tests = list(itertools.chain(
    vars(tests.test_lexer).iteritems(),
    vars(tests.test_ast).iteritems(),
    tests.test_emitters_esdl.__dict__.iteritems(),
    tests.test_model.__dict__.iteritems(),
    tests.test_model_validator.__dict__.iteritems(),
    tests.test_model_ast.__dict__.iteritems()
))

for key, value in all_tests:
    if key.startswith('test_'):
        try:
            total_count += 1
            result = value()
            success_count += 1
        except AssertionError:
            result = None
            print '** Failed:', key
            traceback.print_exc()
            print

        if result is not None:
            total_count -= 1
            success_count -= 1
            for item in result:
                try:
                    total_count += 1
                    key = '%s(%s)' % (item[0].__name__, ', '.join(repr(i) for i in item[1:]))
                    item[0](*item[1:])
                    success_count += 1
                except AssertionError as ex:
                    print ' ** Failed:', key
                    traceback.print_exc()
                    #print str(ex)
                    print

print "Passed %4d/%4d" % (success_count, total_count)
