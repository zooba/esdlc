
PART_1 = [
    '',
    'int wmain(int _argc, wchar_t** _argv) {',
    'int _esdl_iteration_limit = 0;',
    'int _esdl_evaluation_limit = 0;',
    '',
    'bool _esdl_has_iteration_limit = false;',
    'bool _esdl_has_evaluation_limit = false;',
    'bool _esdl_has_fitness_limit = false;',
    'std::wistringstream _esdl_fitness_limit_string;',
    '',
    'bool _esdl_quiet_output = false;',
    'bool _esdl_verbose_output = false;',
    'bool _esdl_csv_output = false;',
    '',
    'int _esdl_seed = 0;',
    '',
    'std::list<std::wstring> _esdl_uninitialised;',
    ''
]

PART_2 = [
    '',
    'int _argi = 0;',
    'for (auto _arg = esdl::get_command(_argc, _argv, _argi); _arg; _arg = esdl::get_command(_argc, _argv, _argi)) {',
    '',    
    'if (_arg.isCommand) {',
    'if (esdl::equals(_arg.variable, L"i", L"iter", L"iterations")) {',
    '_arg >> _esdl_iteration_limit;',
    '_esdl_has_iteration_limit = true;',
    '} else if (esdl::equals(_arg.variable, L"e", L"eval", L"evaluations")) {',
    '_arg >> _esdl_evaluation_limit;',
    '_esdl_has_evaluation_limit = true;',
    '} else if (esdl::equals(_arg.variable, L"f", L"fit", L"fitness")) {',
    '_esdl_fitness_limit_string.str(_arg.value);',
    '_esdl_has_fitness_limit = true;',
    '} else if (esdl::equals(_arg.variable, L"s", L"seed")) {',
    '_arg >> _esdl_seed;',
    '} else if (esdl::equals(_arg.variable, L"csv")) {',
    '_esdl_csv_output = true;',
    '} else if (esdl::equals(_arg.variable, L"v", L"verbose")) {',
    '_esdl_verbose_output = true;',
    '} else if (esdl::equals(_arg.variable, L"q", L"quiet")) {',
    '_esdl_quiet_output = true;',
    '} else {',
    'std::wcout << L"Unrecognised option: /"  << _arg.variable << std::endl;',
    '}'
]

PART_3 = [
    '}',
    '',
    'if (!_esdl_uninitialised.empty()) {',
    'std::wcerr << L"Uninitialised variables:" << std::endl;',
    'std::for_each(std::begin(_esdl_uninitialised), std::end(_esdl_uninitialised), [](std::wstring _var) { std::wcerr << L"    " << _var << std::endl; });',
    'return 1;',
    '}',
    '',
    'esdl::seed(_esdl_seed);',
    '',
]

def get(variables):
    '''Variables is a list of tuples where the first element is the
    safe variable name and the second is the actual name. For example,
    "static" may be passed as ("_safe_static", "static").
    '''
    inits = []
    reads = ['}']
    for name, safe_name in variables:
        inits.append('float %s = 0; _esdl_uninitialised.push_back(L"%s");' % (safe_name, name))
        reads.pop()
        reads.extend((
            '} else if (esdl::equals(_arg.variable, L"%s")) {' % name,
            '_arg >> %s;' % safe_name,
            '_esdl_uninitialised.remove(L"%s");' % name,
            '}'
        ))
    
    return PART_1 + inits + PART_2 + reads + PART_3
