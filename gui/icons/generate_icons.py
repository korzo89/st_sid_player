import png
import glob
import argparse
import os
import sys


def log(msg):
    print(msg)
    sys.stdout.flush()


def convert_file(path, hfile, cfile):
    basename = os.path.basename(path)
    name = os.path.splitext(basename)[0]
    log('Converting icon file %s -> icon_%s' % (basename, name))
    
    reader = png.Reader(filename=path)
    result = reader.read()
    width = result[0]
    height = result[1]
    data = result[2]
    info = result[3]
    depth = info['bitdepth']
    
    rle = []
    samples = []
    for row in data:
        alpha = map(lambda x: (2**depth - 1) - x, row[3::4])
        for x in alpha:
            if (len(samples) == 0) or (samples[-1] == x):
                samples.append(x)
                continue
            
            rle.append((len(samples), samples[0]))
            
            samples = [x]
    
    if len(samples) != 0:
        rle.append((len(samples), samples[0]))
    
    res = []
    joined = []
    for i in rle:
        if i[0] > 3:
            if len(joined) != 0:
                res.append(joined)
            res.append(i)
            joined = []
        else:
            joined.append(i)
    
    if len(joined) != 0:
        res.append(joined)
    
    hfile.write('extern GUI_CONST_STORAGE GUI_BITMAP icon_%s;\n' % name)
    
    cfile.write('static GUI_CONST_STORAGE unsigned char icon_%s_data[] = {\n' % name)
    
    for i in res:
        if isinstance(i, tuple):
            cfile.write('    %d, 0x%02X,\n' % i)
        else:
            num = reduce(lambda n, x: n + x[0], i, 0)
            cfile.write('    0, %d, ' % num)
            for x in i:
                cfile.write(('0x%02X, ' % x[1]) * x[0])
            cfile.write('\n')

    cfile.write('    0\n')
    cfile.write('};\n\n');
    
    cfile.write('GUI_CONST_STORAGE GUI_BITMAP icon_%s = {\n' % name)
    cfile.write('    %d, %d, %d,\n' % (width, height, width))
    cfile.write('    GUI_COMPRESS_RLE8,\n')
    cfile.write('    icon_%s_data,\n' % name)
    cfile.write('    0,\n')
    cfile.write('    GUI_DRAW_RLEALPHA\n')
    cfile.write('};\n\n')
        
        
def convert_directory(path):
    files = glob.glob(os.path.join(path, '*.png'))
    
    hfile = open('icons.h', 'w')
    cfile = open('icons.c', 'w')
    
    hfile.write('/* Auto generated file */\n')
    hfile.write('#ifndef ICONS_H\n')
    hfile.write('#define ICONS_H\n\n')
    hfile.write('#include <GUI.h>\n\n')
    
    cfile.write('/* Auto generated file */\n')
    cfile.write('#include "icons.h"\n\n')
    
    for f in files:
        convert_file(f, hfile, cfile)
        
    hfile.write('\n#endif\n')
    
    hfile.close()
    cfile.close()
    
        
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('directory', type=str, help='Icon PNG files directory')
    args = parser.parse_args()
    
    convert_directory(args.directory)
    