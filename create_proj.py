import shutil
import os
import sys


BUILD_DIR = 'build' 


def log(msg):
    print(msg)
    sys.stdout.flush()

if __name__ == '__main__':
    log('Regenerating directory "%s"...' % BUILD_DIR)
    shutil.rmtree(BUILD_DIR, ignore_errors=True)
    os.mkdir(BUILD_DIR)
    os.chdir(BUILD_DIR)
    
    log('Executing cmake...')
    os.system('cmake .. -G "Unix Makefiles"')
    