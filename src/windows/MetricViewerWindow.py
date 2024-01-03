import tkinter as tk
from src.windows.SimpleWindow import SimpleWindow
from src.windows.MetricGraphWindow import MetricGraphWindow

STANDARD_OUTPUT_OPERATION_CODE_SET_METRIC = "0"
#define STANDARD_OUTPUT_OPERATION_CODE_SET_LED = 1
#define STANDARD_OUTPUT_OPERATION_CODE_SHOW_LEDS = 2
#define STANDARD_OUTPUT_OPERATION_CODE_CLEAR_LEDS = 3

class MetricViewerWindow(SimpleWindow):
    keyToIndexMap = {}
    indexToKeyMap = {}
    graphWindow = None
    graphedMetricName = ""
    keyToUserFriendlyNameMap = {}
    keyToUserFriendlyNameMap[0] = "unknown"
    keyToUserFriendlyNameMap[1 ] = "usingMirroredTransformMaps1"
    keyToUserFriendlyNameMap[2 ] = "usingMirroredTransformMaps2"
    keyToUserFriendlyNameMap[3 ] = "primaryEffectToggle"
    keyToUserFriendlyNameMap[4 ] = "remoteOpCode"
    keyToUserFriendlyNameMap[5 ] = "renderTime"
    keyToUserFriendlyNameMap[6 ] = "fxA1AudioLevelThresholdToShowMoreIntenseEffect"
    keyToUserFriendlyNameMap[7 ] = "fxB1AudioLevelThresholdToShowMoreIntenseEffect"
    keyToUserFriendlyNameMap[8 ] = "transitionDirection"
    keyToUserFriendlyNameMap[9 ] = "desiredMillisecondTransitionDuration"
    keyToUserFriendlyNameMap[10] = "fxBrightnessMode"
    keyToUserFriendlyNameMap[11] = "fxSize"
    keyToUserFriendlyNameMap[12] = "exponentialMovingAverage"
    keyToUserFriendlyNameMap[13] = "currentRootMeanSquare"
    keyToUserFriendlyNameMap[14] = "currentAudioIntensityLevel"
    keyToUserFriendlyNameMap[15] = "fxTime1"
    keyToUserFriendlyNameMap[16] = "fxTime2"

    keyToUserFriendlyNameMap[17] = "fxTimeMode1"
    keyToUserFriendlyNameMap[18] = "fxTimeMode2"
    keyToUserFriendlyNameMap[19] = "percentOfOldMixingModeToMixIn8Bit"
    keyToUserFriendlyNameMap[20] = "millisecondsLeftInMixingModeBlend"
    keyToUserFriendlyNameMap[21] = "mixingModeBlendFunction"
    keyToUserFriendlyNameMap[22] = "millisecondsLeftInMixingModeBlendTotalDuration"
    keyToUserFriendlyNameMap[23] = "fxTransformMap1"
    keyToUserFriendlyNameMap[24] = "remoteOpCode"
    keyToUserFriendlyNameMap[25] = "totalFrameTime"
    keyToUserFriendlyNameMap[26] = "percentOfEffectBToShow8Bit"
    keyToUserFriendlyNameMap[27] = "msFor2ndTo1stEffect"
    keyToUserFriendlyNameMap[28] = "percentSecondaryEffectToShow"
    keyToUserFriendlyNameMap[29] = "fxPaletteOffset"
    keyToUserFriendlyNameMap[30] = "peakDetector"
    keyToUserFriendlyNameMap[31] = "beatCount"
    keyToUserFriendlyNameMap[32] = "fxsRandomizedCount"
    keyToUserFriendlyNameMap[33] = "currentScreenMap"
    keyToUserFriendlyNameMap[34] = "fxCurrentPaletteOffsetTarget"
    keyToUserFriendlyNameMap[35] = "fxPalette"
    keyToUserFriendlyNameMap[36] = "fxPaletteOffset"
    keyToUserFriendlyNameMap[36] = "profileTime"
    keyToUserFriendlyNameMap[37] = "fadeSettingAmount"

    def __init__(self):
        self.createRoot()
        self.root.geometry("+0+0")

    def sortMetricsList(self):
        newKeyToIndexMap = {}
        self.indexToKeyMap = {}
        metricNamesList = list(self.keyToIndexMap.keys())
        metricNamesList.sort()
        newIndex = 0

        currentValues = {}
        for currentMetric in self.keyToIndexMap:
            currentValues[currentMetric] = self.listbox.get(self.keyToIndexMap[currentMetric])

        for metricName in metricNamesList:
            newKeyToIndexMap[metricName] = newIndex
            self.indexToKeyMap[newKeyToIndexMap[metricName]] = metricName
            self.listbox.delete(newIndex)
            self.listbox.insert(newIndex, currentValues[metricName])
            newIndex += 1
        self.keyToIndexMap = newKeyToIndexMap

    def getOrAssignIndexForKey(self, key):
        if key in self.keyToIndexMap: 
            return self.keyToIndexMap[key]
        self.keyToIndexMap[key] = len(self.keyToIndexMap)
        self.indexToKeyMap[self.keyToIndexMap[key]] = key
        return self.keyToIndexMap[key]

    def update_single_status(self, key, newStatus):
        if (self.graphWindow is None or self.graphWindow.isClosed):
            index = self.getOrAssignIndexForKey(key)
            self.listbox.delete(index)
            self.listbox.insert(index, newStatus)
            if key == self.graphedMetricName:
                self.listbox.selection_set(first=index)
                self.listbox.yview(index)
                self.listbox.activate(index)

    def runCommand(self, arguments):
        if arguments[0] == STANDARD_OUTPUT_OPERATION_CODE_SET_METRIC:
            if len(arguments) == 3:
                self.update_single_status(arguments[1], f"{self.keyToUserFriendlyNameMap[int(arguments[1])]}: {arguments[2]}")
            if self.graphWindow is not None and arguments[1] == self.graphedMetricName:
                try:
                    self.graphWindow.insertData(float(arguments[2]))
                except Exception as e:
                    print(e)

    def createRoot(self):
        super().createRoot()
        self.root.title("Metric Status Display")
        self.root.configure(bg='black', height=500)

        self.custom_font = ('Consolas', 9)
        self.custom_fg = 'white'

        self.frame = tk.Frame(self.root, bg='black')
        self.frame.pack(padx=10, pady=10)

        self.listbox = tk.Listbox(self.frame, font=self.custom_font, fg=self.custom_fg, bg='black', width=90, height=80, borderwidth=0, highlightthickness=0, activestyle=None)
        self.listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        self.listbox.bind("<<ListboxSelect>>", self.onSelect)

        sortButton = tk.Button(self.root, text='Sort metrics list', background="black", foreground="white", width=100, command=self.sortMetricsList)
        sortButton.pack()

    def update(self):
        if self.root is not None: 
            self.root.update()
        if (self.graphWindow is not None): 
            self.graphWindow.update()

    def onSelect(self, event):
        index = event.widget.curselection()[0]
        if index >= 0 and index < len(self.indexToKeyMap):
            key = self.indexToKeyMap[index]
        self.graphedMetricName = key
        if (self.graphWindow is None or self.graphWindow.isClosed):
            self.graphWindow = MetricGraphWindow()
        else:
            pass
        self.graphWindow.resetMinMax()
