from tests.LedGridWindow import LedGridWindow
from tests.ConfigurationStatusWindow import ConfigurationStatusWindow
from tests.RemoteControlWindow import RemoteControlWindow
from subprocess import Popen, PIPE, STDOUT
import psutil
from Configuration.configuration import PhysicalLedIndexMappingWidth, PhysicalLedIndexMapping, PhysicalLedIndexMappingHeight
import os

def parseLine(line):
    try:
        preLine = str(line)[2:-5].strip()
        arguments = preLine.split(",")
        return arguments
    except Exception as e:
        print(repr(e))
        return []

def send_input(process, user_input):
    pass

os.system("make")
ledGridWindow = LedGridWindow()
configurationStatusWindow = ConfigurationStatusWindow()
remoteControlWindow = RemoteControlWindow()
program = Popen('bin\\wings.exe', stdout = PIPE, stderr = STDOUT, stdin = PIPE, shell = True)

while True:
    ledGridWindow.update()
    if ledGridWindow.isClosed: break

    configurationStatusWindow.update()
    if configurationStatusWindow.isClosed: break

    remoteControlWindow.update()
    if remoteControlWindow.isClosed: break
    
    line = program.stdout.readline()
    if not line: break
    
    arguments = parseLine(line)
    ledGridWindow.runCommand(arguments)
    configurationStatusWindow.runCommand(arguments)

    send_input(program, "Hello World")

parent = psutil.Process(program.pid)
for child in parent.children(recursive=True):
    child.kill()
    child.wait()
program.kill()
program.wait()

try:
    ledGridWindow.onClosing()
    ledGridWindow.destroy()
except:
    pass
try:
    configurationStatusWindow.onClosing()
    configurationStatusWindow.destroy()
except:
    pass
try:
    remoteControlWindow.onClosing()
    remoteControlWindow.destroy()
except:
    pass
