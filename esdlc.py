import argparse
import sys
from esdlc import compileESDL
import esdlc.emitters

VERSION = '0.0.0.1'

LOGO = r'''ESDL Multi-targeting Compiler %s
Copyright (c) 2011 Steve Dower. All rights reserved.
See LICENCE.TXT for licencing information.
''' % VERSION

def main(args):
    parser = argparse.ArgumentParser(prefix_chars='/', fromfile_prefix_chars='@', add_help=False)
    parser.add_argument('/nologo',
                        dest='logo', action='store_false',
                        help='Suppress copyright message')
    parser.add_argument('/help', '/?',
                        dest='show_help', action='store_true',
                        help='Display help information')

    parser.add_argument('/version', '/v',
                        action='version', version=VERSION,
                        help='Display version number only')
    parser.add_argument('/emitter', '/e', metavar='EMITTER',
                        dest='emitters', action='append',
                        help='The name of the emitter to use.')

    parser.add_argument('/O', nargs='?', type=int, metavar='#',
                        dest='optimise_level', action='store', default=0, const=99,
                        help='Optimisation level (default=0)')
    parser.add_argument('/P',
                        dest='profile', action='store_true',
                        help='Include profiling support')

    parser.add_argument('/o', dest='outfiles', nargs=1, metavar='FILE',
                        action='append',
                        help='Destination file (default=console)')
    parser.add_argument('infile', nargs='?',
                        help='The source file name (usually *.esdl)')

    opts = parser.parse_args(args)

    if opts.show_help:
        parser.print_help()
        return 0

    if opts.logo:
        print LOGO

    if not opts.infile:
        print 'No source file provided.'
        return 1

    emitters = []
    for name in opts.emitters or []:
        name = name.strip(': .,/=[]{}\\()').lower()
        if name == 'ast':
            emitters.append(('ast', None))
            continue

        try:
            root = __import__('esdlc.emitters.' + name)
            for bit in ['emitters'] + name.split('.'):
                root = getattr(root, bit)
            func = getattr(root, 'emit')
            emitters.append((name, func))
        except ImportError:
            print 'Unknown emitter ' + name

    if opts.outfiles:
        outpaths = [i[0].strip(':\\/"') for i in opts.outfiles]
        outfiles = [open(i, 'w') for i in outpaths]
    else:
        outpaths = [None]
        outfiles = [sys.stdout]
    outfile_i = 0
    
    model, valid = compileESDL(opts.infile)

    for name, emitter in emitters:
        if emitter is None:
            if opts.optimise_level < 99:
                outfiles[outfile_i].write(model.ast.format())
            else:
                outfiles[outfile_i].write(str(model.ast))
        else:
            code, context = emitter(model, out=outfiles[outfile_i], optimise_level=opts.optimise_level, profile=opts.profile)
            if hasattr(context, 'get'):
                compile_callback = context.get('__compile')
                if hasattr(compile_callback, '__call__') and outpaths[outfile_i]:
                    outfiles[outfile_i].close()
                    compile_callback(outpaths[outfile_i])
                    outfiles[outfile_i] = open(outpaths[outfile_i], 'a')
        outfile_i = (outfile_i + 1) % len(outfiles)

if __name__ == '__main__':
    #sys.exit(main(sys.argv[1:]))
    main(sys.argv[1:])


