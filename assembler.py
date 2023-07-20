from opcodes import opcodesf
import os, sys, struct, platform

command_length = 2

class Bytecode:

    fmt = '<i'
    def __init__(self):
        self.bytecode = bytearray()
        
    def append(self, value):
        self.bytecode += struct.pack(self.fmt, value)
        
    def save(self, source):
        filename = source + '.zl'
        with open(filename, 'bw') as output_file:
            output_file.write(self.bytecode)
        print('Bytecode size:', len(self))
        return filename
    	
    def __len__(self):
        return len(self.bytecode)

    def __setitem__(self, addr, value):
    	self.bytecode[addr : addr + command_length] = struct.pack(self.fmt, value)
        
        
def die(message, linenum = None):
    if linenum == None:
        print('Error: ' + message + ', exiting')
    else:
        print('Error: ' + message + ' at line ' + str(linenum + 1) + ', exiting')    
    sys.exit(1)

if len(sys.argv) < 2:
    die('not specified input file, please specify it in first argument')
bytecode = Bytecode()
labels = {}
vars = {}
source = sys.argv[1]
print('Assembling...')
with open(source) as source_file:
    for linenum, line in enumerate(source_file):
        line = line.strip()
        if not line or line[0] in ('#', '/'): 
            continue
        command, *arguments = line.split()
        command = command.upper()

        if command not in opcodesf:
            die('Unknown command ' + command, linenum)
        
        bytecode.append(opcodesf.index(command))
        
        if command in ('PUSH'): #mem instruction can be here
            if not arguments:
                die(command + ' needs 1 argument', linenum)
            if arguments[0].isnumeric():
                val = int(arguments[0])
                bytecode.append(val)
            else:
                var = arguments[0]
                die('not a number ' + var, linenum)

    bytecode.append(opcodesf.index('QUIT'))

for label in labels.values():
    label.set_transitions(bytecode)

filename = bytecode.save(source)
print('Launching...\n')

if platform.system() == 'Linux':
	command_to_launch = os.path.join(os.path.abspath(os.path.dirname( \
                     sys.argv[0])), 'stack_machine')
	os.system(command_to_launch + ' ' + filename + '')
else: # windows
	command_to_launch = os.path.join(os.path.abspath(os.path.dirname( \
                     sys.argv[0])), 'stack_machine.exe')
	os.system('""' + command_to_launch + '" "' + filename + '""')

