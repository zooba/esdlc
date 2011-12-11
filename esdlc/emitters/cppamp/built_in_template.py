import datetime

def get(source_file):
    return [
        '//',
        '// Generated from: %s' % source_file,
        '// At: %s' % datetime.datetime.now(),
        '//',
        '',
        ''
    ]
