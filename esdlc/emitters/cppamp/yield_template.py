
DEFAULT_GROUP_TEMPLATE = [
    'esdl::_default_statistics_name = L"$group$";',
    'esdl::tt::fitness_type<decltype($safe_group$)>::type _esdl_fitness_limit;',
    'if (_esdl_has_fitness_limit) { ',
    '_esdl_fitness_limit_string >> _esdl_fitness_limit;',
    '}',
    'if (!_esdl_quiet_output) {',
    'esdl::write_statistics_header<typename esdl::tt::individual_type<decltype($safe_group$)>::type>();',
    '}',
]

YIELD_TEMPLATE = [
    'if (!_esdl_quiet_output && _esdl_verbose_output) {',
    'if (_esdl_csv_output) esdl::write_group_raw(L"$group$", _esdl_$safe_group$_group);',
    'else esdl::write_group(L"$group$", _esdl_$safe_group$_group);',
    '}',
]

def get(group_name, safe_group_name, is_default=False, is_first=False, init_only=False):
    result = []
    if is_default and is_first:
        result.extend(s.replace('$safe_group$', safe_group_name).replace('$group$', group_name) for s in DEFAULT_GROUP_TEMPLATE)
    
    result.append('%s.evaluate();' % safe_group_name)
    if is_first:
        result.append('auto _esdl_%s_group = esdl::make_stats(L"%s", %s);' % (safe_group_name, group_name, safe_group_name))
    else:
        result.append('_esdl_%s_group = esdl::make_stats(L"%s", %s);' % (safe_group_name, group_name, safe_group_name))

    if not init_only:
        result.extend(s.replace('$safe_group$', safe_group_name).replace('$group$', group_name) for s in YIELD_TEMPLATE)
    return result
