from src.windows.LedGridWindow import LedGridWindow
from src.windows.TraceViewerWindow import TraceViewerWindow
from src.windows.RemoteControlWindow import RemoteControlWindow
from subprocess import Popen, PIPE, STDOUT
import psutil
import os

def parseLine(line):
    try:
        preLine = str(line)[2:-5].strip()
        arguments = preLine.split(",")
        int(arguments[0])
        return arguments
    except Exception as e:
        print(f"Error parsing program output: {line}")
        return []

def main():
    if os.system("make") != 0:
        exit()
    ledGridWindow = LedGridWindow()
    configurationStatusWindow = TraceViewerWindow()
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
        if len(arguments) > 0:
            ledGridWindow.runCommand(arguments)
            configurationStatusWindow.runCommand(arguments)

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
