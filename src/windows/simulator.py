from src.windows.LedGridWindow import LedGridWindow
from src.windows.MetricViewerWindow import MetricViewerWindow
from src.windows.RemoteControlWindow import RemoteControlWindow
from src.windows.paletteViewerWindow import PaletteViewerWindow
from subprocess import Popen, PIPE, STDOUT
import psutil
import os

def parseLine(program):
    try:
        line = program.stdout.readline()
        if not line: 
            return []
        preLine = str(line)[2:-5].strip()
        arguments = preLine.split(",")
        int(arguments[0])
        return arguments
    except Exception as e:
        print(e)
        print(f"Error parsing program output: {line}")
        maxErrorLinesToPrint = 100
        linesPrinted = 0
        while linesPrinted < maxErrorLinesToPrint:
            line = program.stdout.readline()
            if not line: 
                return []
            print(line)
            linesPrinted += 1
        return []

def main():
    processPath = 'bin\\wings.exe'
    if os.system("make") != 0:
        exit()
    ledGridWindow = LedGridWindow()
    configurationStatusWindow = MetricViewerWindow()
    remoteControlWindow = RemoteControlWindow()
    paletteViewerWindow = PaletteViewerWindow()
    program = Popen(processPath, stdout = PIPE, stderr = STDOUT, stdin = PIPE, shell = True)

    while True:
        ledGridWindow.update()
        if ledGridWindow.isClosed: 
            break

        configurationStatusWindow.update()
        if configurationStatusWindow.isClosed: 
            break

        remoteControlWindow.update()
        if remoteControlWindow.isClosed: 
            break
        
        paletteViewerWindow.update()
        if paletteViewerWindow.isClosed: 
            break

        arguments = parseLine(program)
        if len(arguments) > 0:
            ledGridWindow.runCommand(arguments)
            configurationStatusWindow.runCommand(arguments)
            paletteViewerWindow.runCommand(arguments)
        else:
            break

    try:
        parent = psutil.Process(program.pid)
        for child in parent.children(recursive=True):
            child.kill()
            child.wait()
        program.kill()
        program.wait()
    except psutil.NoSuchProcess:
        print(f"Simulator Error: {processPath} died before the simulator could clean it up gracefully.")

    try:
        ledGridWindow.onClosing()
        ledGridWindow.destroy()
    except Exception as e:
        print(e)
    try:
        configurationStatusWindow.onClosing()
        configurationStatusWindow.destroy()
    except Exception as e:
        print(e)
    try:
        remoteControlWindow.onClosing()
        remoteControlWindow.destroy()
    except Exception as e:
        print(e)
