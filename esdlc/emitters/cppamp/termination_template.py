
# TODO: Replace _getch with stdin read
TERMINATION_TEMPLATE = [
    'esdl::EndReason _reason;',
    'for (;; _iteration += 1) {',
    'if (!_esdl_quiet_output) {',
    'esdl::write_statistics(esdl::default_statistics, _esdl_$default_group$_group.front());',
    '}',
    '',
    'if (!(_esdl_has_iteration_limit || _esdl_has_evaluation_limit || _esdl_has_fitness_limit)) {',
    'int c = esdl::getch();',
    'if (c == \'x\') break;',
    '}',
    'else if (_esdl_has_iteration_limit && _iteration > _esdl_iteration_limit) {',
    '_reason = esdl::IterLimit;',
    'break;',
    '} else if (_esdl_has_evaluation_limit && _evaluations > _esdl_evaluation_limit) {',
    '_reason = esdl::EvalLimit;',
    'break;',
    '} else if (_esdl_has_fitness_limit && _esdl_$default_group$_group.front().fitness > _esdl_fitness_limit) {',
    '_reason = esdl::FitLimit;',
    'break;',
    '}',
]

TERMINATION2_TEMPLATE = [
    'esdl::EndReason _reason;',
    'for (;; _iteration += 1) {',
    'if (!(_esdl_has_iteration_limit || _esdl_has_evaluation_limit)) {',
    'int c = _getch();',
    'if (c == \'x\') break;',
    '}',
    'else if (_esdl_has_iteration_limit && _iteration > _esdl_iteration_limit) {',
    '_reason = esdl::IterLimit;',
    'break;',
    '} else if (_esdl_has_evaluation_limit && _evaluations > _esdl_evaluation_limit) {',
    '_reason = esdl::EvalLimit;',
    'break;',
    '}',
]

def get(default_group):
    if default_group:
        return [s.replace('$default_group$', default_group) for s in TERMINATION_TEMPLATE]
    else:
        return TERMINATION2_TEMPLATE
