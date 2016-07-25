import shutil
import os
import sys
import argparse


def log(msg):
    print(msg)
    sys.stdout.flush()
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    
    default_output_dir = 'build'
    parser.add_argument('-o', '--output', type=str, default=default_output_dir, 
                        help='CMake output directory path, defaults to "%s"' % default_output_dir)
    
    args = parser.parse_args()
    
    log('Regenerating directory "%s"...' % args.output)
    shutil.rmtree(args.output, ignore_errors=True)
    os.mkdir(args.output)
    os.chdir(args.output)
    
    log('Executing cmake...')
    script_path = sys.path[0]
    os.system('cmake %s -G "Unix Makefiles"' % script_path)
    