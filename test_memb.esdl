x = config.member
x = config.member.submember
x = config.func(param=value)
x = config.member.func(param1=value, param2=value)
x = config.func(param=value).submember
x = config.func(param=value).submember.subsubmember
x = config.func(param1=value).subfunc(param2=value)
x = config[index]
x = config.member[index]
x = config[index].member

EVAL pop USING config.member
