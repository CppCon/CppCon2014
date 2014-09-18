import os,time,sys

spaceships = ['spaceship1', 'spaceship2']
makes = ['make1', 'make2', 'make3']

cmd = 'clang++ -w -std=c++1y -stdlib=libc++ test.cc -o a.out -ftemplate-depth=2000 ' + ' '.join(sys.argv[1:])

def profile(cmd):
    start = time.time()
    os.system(cmd)
    os.system(cmd)
    os.system(cmd)
    end = time.time()
    return (end - start) / 3

print '     ',
for make in makes:
    for spaceship in spaceships:
        print '%12s' % make,
print ''
print '     ',
for make in makes:
    for spaceship in spaceships:
        print '%12s' % spaceship,
print ''

for parameter in [1,2,4,8,16,32,64,128,256,512,1024]:

    # Profile each combination of SPACESHIP and MAKE, for parameter value "i".
    print '%4d:' % parameter,
    for make in makes:
        for spaceship in spaceships:
            t = profile('%s -DSPACESHIP=%s -DMAKE=%s -DPARAMETER=%d' % (cmd, spaceship, make, parameter))
            print '%12.4f' % t,
    print ''
