import sys, os, time
import json
import codecs
import threading
from PyQt4 import QtGui, QtCore
from PyQt4 import uic
from PyQt4.QtGui import QMainWindow, QApplication, QColor, QAbstractItemView,\
		QSyntaxHighlighter, QTextCharFormat, QFileDialog, QTableWidgetItem
from PyQt4.QtCore import Qt, QRegExp, SIGNAL, QFile
from utils import Instruction, Interpret

UI_MainWindow, QtBaseClass = uic.loadUiType("swCompiler.ui")
UI_RtmWindow, RuntimeClass = uic.loadUiType("runWindow.ui")

qssFile = QFile("themestyle.qss")
qssFile.open(QFile.ReadOnly)
themeQss = qssFile.readAll()
themeQss = unicode(themeQss, encoding="utf8")

'''
	delete temporary files after a build work finished
'''
def cleanfiles():
	from os.path import isfile
	if isfile(".\\~.tmp"):
		os.remove(".\\~.tmp")
	if isfile(os.getcwd()+"\\ferr.json"):
		os.remove(os.getcwd()+"\\ferr.json")
	if isfile(os.getcwd()+"\\fcode.json"):
		os.remove(os.getcwd()+"\\fcode.json")

'''
	ferr.json can be loaded for error message and knowing if the build work is succeed
	This function is used to check out if the ferr.json is existed
'''
def judge():
	from os.path import isfile
	startTime = time.time()
	while True:
		if isfile(os.getcwd()+"\\ferr.json"):
			return True
		if time.time()-startTime > 3:
			return False
		time.sleep(0.1)

'''
	This function is the real processing for backstage-interpretation
	and it should work in a new thread so that I/O cannot block the UI
'''
def procRun(codeList, window):
	window.interpret = Interpret(codeList)
	run = window.interpret
	mod = window.mod
	window.setStackValues(run.showStack())
	while True:
		# tip the current processing TAC code
		window.setCodeStatus(run.p, True)

		tag = run.sg_step()
		window.setStackValues(run.showStack())
		if tag == 1:	#input
			window.input()
		if tag == 2:	#output
			window.output(run.recv())

		if window.stopsgl == 1:
			window.setCodeStatus(run.c, False)
			break
		if run.judge() == False:
			break

		if mod == 1:
			while window.debug == 0:
				time.sleep(0.05)
			window.setCodeStatus(run.c, False)
			if window.debug == 1:	# next step
				pass
			if window.debug == 2:	# step into
				pass
			if window.debug == 3:	# over step
				mod = 0
				window.setDebugEnabled(False)
			if window.debug == 4:	# step out
				run.paramInit()
				window.RuntimePad.clear()
				window.RuntimePad.textCursor().insertText("")
				window.debug = 0
				continue
			window.debug = 0
		# cancel the tip for TAC code just processing
		window.setCodeStatus(run.c, False)
		
	window.setDebugEnabled(False)
	window.actionStop.setEnabled(False)
	window.output("\n=== the processing is over ===")


'''
	extending the QSyntaxHighlighter class for highlight the sw texts
'''
class SWHighlighter(QSyntaxHighlighter):

	Rules = []
	Formats = {}

	def __init__(self, parent=None):
		super(SWHighlighter, self).__init__(parent)
		self.initializeFormats()

		KEYWORDS = ["var",		"let",	"func",		"if",		"else",
					"for",		"in",	"while",	"repeat",	"read",
					"print",	"call",	"return",	"break",	"continue"]

		OPERATORS = ["+", "-", "*", "/", "%", "&", "|", "~", "^", "!",
					 "<", ">", "=", ".."]

		SWHighlighter.Rules.append((QRegExp(
			"|".join([r"\b%s\b" % keyword for keyword in KEYWORDS])),
			"keyword"))
		SWHighlighter.Rules.append((QRegExp(
			r"\b[0-9]+\b"),
			"number"))
		SWHighlighter.Rules.append((QRegExp(
			r"/\*.*\*/"),
			"comment"))
		
	@staticmethod
	def initializeFormats():
		baseFormat = QTextCharFormat()
		baseFormat.setFontFamily("Consolas")
		baseFormat.setFontPointSize(12)
		for name, color in (("normal", Qt.white),
			("keyword", Qt.yellow), ("builtin", Qt.blue), 
			("comment", Qt.gray), ("number", Qt.yellow)):
			format = QTextCharFormat(baseFormat)
			format.setForeground(QColor(color))
			if name == "comment":
				format.setFontItalic(True)
			SWHighlighter.Formats[name] = format

	def highlightBlock(self, text):
		NORMAL, TRIPLESINGLE, TRIPLEDOUBLE = range(3)

		textLength = len(text)
		prevState = self.previousBlockState()

		self.setFormat(0, textLength, SWHighlighter.Formats["normal"])

		for regex, format in SWHighlighter.Rules:
			i = regex.indexIn(text)
			while i >= 0:
				length = regex.matchedLength()
				self.setFormat(i, length, SWHighlighter.Formats[format])
				i = regex.indexIn(text, i + length)

		if not text:
			pass
		else:
			stack, pre = [], None
			for i, c in enumerate(text):
				if c == "/" and pre == c:
					self.setFormat(i-1, len(text)-i+1, SWHighlighter.Formats["comment"])
					break
				pre = c

		self.setCurrentBlockState(NORMAL)

	def rehighlight(self):
		QApplication.setOverrideCursor(QCursor(Qt.WaitCursor))
		QSyntaxHighlighter.rehighlight(self)
		QApplication.restoreOverrideCursor()

class RuntimeWin(QMainWindow, UI_RtmWindow):
	def __init__(self, mod = 0, parent = None):
		QMainWindow.__init__(self)
		UI_RtmWindow.__init__(self)
		self.setupUi(self)
		self.mod = mod
		self.debug = 0
		self.parent = parent
		self.iptsgl = 0
		self.stopsgl = 0
		self.initUI()

	def output(self, s):
		if type(s) == type(1):
			s = str(s)
		self.RuntimePad.textCursor().insertText(s+'\n')

	def input(self):
		self.RuntimePad.textCursor().insertText(">>>\t")
		self.InputTextPad.setReadOnly(False)
		ipt = None
		while self.iptsgl == 0:
			if self.stopsgl == 1:
				self.InputTextPad.setText("")
				self.InputTextPad.setEnabled(False)
				return
			time.sleep(0.05)
		self.iptsgl = 0
		ipt = self.InputTextPad.text()
		self.interpret.send(ipt.toInt()[0])
		self.RuntimePad.textCursor().insertText(ipt+"\n")
		self.InputTextPad.setText("")
		self.InputTextPad.setReadOnly(True)

	def inputwait(self):
		self.iptsgl = 1

	def setDebugEnabled(self, ok):
		self.actionStepnext.setEnabled(ok)
		self.actionStepinto.setEnabled(ok)
		self.actionStepover.setEnabled(ok)
		self.actionStepout.setEnabled(ok)

	def StopProc(self):
		self.stopsgl = 1
		self.debug = 3
	def Stepnext(self):
		self.debug = 1
	def Stepinto(self):
		self.debug = 2
	def Stepover(self):
		self.debug = 3
	def Stepout(self):
		self.debug = 4

	def setCodeStatus(self, idx, ok):
		item = self.TACCodeLists.item(idx+1)
		if ok == True:
			item.setBackgroundColor(Qt.red)
		else:
			item.setBackgroundColor(QColor(150, 150, 140))

	def setStackValues(self, stk):
		if self.mod == 1:
			self.StackValuesLists.clear()
			for i in xrange(len(stk)):
				self.StackValuesLists.addItem("{0}\t{1}".format(i, stk[i]))

	def initUI(self):
		self.actionStop.triggered.connect(self.StopProc)
		self.actionStepnext.triggered.connect(self.Stepnext)
		self.actionStepinto.triggered.connect(self.Stepinto)
		self.actionStepover.triggered.connect(self.Stepover)
		self.actionStepout.triggered.connect(self.Stepout)

		self.InputTextPad.setReadOnly(True)
		self.connect(self.InputTextPad, SIGNAL("returnPressed()"), self.inputwait)

		if self.mod == 0:
			self.setDebugEnabled(False)
			self.RuntimeStkDock.hide()

		fctData = codecs.open(os.getcwd()+"\\fcode.json", 'r', 'utf-8').read()
		fctData = json.loads(fctData)
		TACLists = []
		idx = 0
		for code in fctData[u"codes"]:
			TAC_T = code[u"code"]
			TAC = Instruction(TAC_T[u"f"], TAC_T[u"l"], TAC_T[u"a"])
			self.TACCodeLists.addItem("{0}\t({1}, {2}, {3})"\
				.format(idx, TAC.f, TAC.l, TAC.a))
			TACLists.append(TAC)
			idx += 1
		self.codeList = TACLists
		self.show()    	

	def closeEvent(self, event):
		if self.parent is not None:
			self.parent.setBuildEnabled(True)
		cleanfiles()

class SWCompiler(QMainWindow, UI_MainWindow):
	def __init__(self):
		QMainWindow.__init__(self)
		UI_MainWindow.__init__(self)
		self.setupUi(self)
		self.highlighter = SWHighlighter(self.codeTextEdit.document())
		self.initUI()

	def errTbInit(self):
		'''
			This function is used to initialize the errMsgTable
		'''
		self.errorMsgTable.clear()
		self.errorMsgTable.setColumnCount(3)
		self.errorMsgTable.setRowCount(1)
		self.errorMsgTable.setHorizontalHeaderLabels(['errno', 'line', 'message'])
		self.errorMsgTable.verticalHeader().setVisible(False)
		self.errorMsgTable.setEditTriggers(QAbstractItemView.NoEditTriggers)
		self.errorMsgTable.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.errorMsgTable.setColumnWidth(0, 70)
		self.errorMsgTable.setColumnWidth(2, 595)
		for idx in range(self.errorMsgTable.columnCount()):
			headItem = self.errorMsgTable.horizontalHeaderItem(idx)
			headItem.setBackgroundColor(QColor(37, 59, 118))
			headItem.setForeground(QColor(0, 0, 0))

	def fileInit(self):
		self.filetag = False
		self.filepath = os.getcwd()
		self.filename = ""
		self.workPathLabel.setText("")
		cleanfiles()

	def initUI(self):
		self.fileInit()
		self.errTbInit()
		self.actionNew.triggered.connect(self.newFile)
		self.actionOpen.triggered.connect(self.openFile)
		self.actionSave.triggered.connect(self.saveFile)
		self.actionBuildAndRun.triggered.connect(self.BuildAndRun)
		self.actionDebug.triggered.connect(self.DebugMod)
		
		self.show()

	def setBuildEnabled(self, ok):
		self.actionBuildAndRun.setEnabled(ok)
		self.actionDebug.setEnabled(ok)

	def startBuild(self):
		'''
			Preparation for build&run or debug a processing
		'''
		self.setBuildEnabled(False)

		text = self.codeTextEdit.toPlainText()
		text = unicode(text.toUtf8(), 'utf-8', 'ignore')
		if text == "":
			text = u" "
		# If the current working codefile is existed, use it directly
		curfile = self.filepath+'\\'
		if self.filetag == True:
			curfile = curfile+self.filename
		# If the current working codefile is new, used a temporary file
		else:
			curfile = curfile+"~.tmp"

		codecs.open(curfile, 'w', 'utf-8').write(text)
		os.system(os.getcwd()+"\\swcpl.exe "+curfile)

	def runOver(self):
		self.setBuildEnabled(True)

	def errTbBuild(self):
		'''
			This function is to get error messages and fill the errorMsgTable
			return:  errNum
		'''
		errData = codecs.open(os.getcwd()+"\\ferr.json", 'r', 'utf-8').read()
		errData = json.loads(errData)

		idx = 0
		self.errTbInit()
		self.errorMsgTable.setItem(idx, 2, QTableWidgetItem(errData[u"total"]))
		for err in errData[u'errors']:
			self.errorMsgTable.insertRow(idx)
			self.errorMsgTable.setItem(idx, 0, QTableWidgetItem(err[u'typeno']))
			self.errorMsgTable.setItem(idx, 1, QTableWidgetItem(err[u'line']))
			self.errorMsgTable.setItem(idx, 2, QTableWidgetItem(err[u'message']))
			idx += 1
		return errData[u'errNum']

	def BuildAndRun(self):
		self.startBuild()
		
		if judge() == True:
			errNum = self.errTbBuild()
			if errNum == 0:
				self.runDlg = RuntimeWin(0, self)
				self.IOthread = threading.Thread(target=procRun,\
					args=(self.runDlg.codeList, self.runDlg))
				#self.IOthread.setDaemon('True')
				self.IOthread.start()
		else:
			QMessageBox.critical(self, "Critical", self.tr("Compiler processing error"))

	def DebugMod(self):
		self.startBuild()
		
		if judge() == True:
			errNum = self.errTbBuild()
			if errNum == 0:
				self.runDlg = RuntimeWin(1, self)
				self.IOthread = threading.Thread(target=procRun,\
					args=(self.runDlg.codeList, self.runDlg))
				#self.IOthread.setDaemon('True')
				self.IOthread.start()
		else:
			QMessageBox.critical(self, "Critical", self.tr("Compiler processing error"))		

	def newFile(self):
		self.fileInit()
		self.codeTextEdit.setPlainText("")

	def openFile(self):
		dirO = QFileDialog.getOpenFileName(self, "Open Exist File", self.filepath, \
			"sw Files (*.sw);;Text Files (*.txt)")
		if dirO != "":
			from os.path import isfile
			if isfile(dirO):
				text = codecs.open(dirO, 'r', 'utf-8').read()
				self.codeTextEdit.setPlainText(text)

				dirO = unicode(dirO.toUtf8(), 'utf-8', 'ignore')
				self.filepath, self.filename = os.path.split(dirO)
				self.filetag = True
				self.workPathLabel.setText(self.filepath)

	def saveFile(self):
		text = self.codeTextEdit.toPlainText()
		text = unicode(text.toUtf8(), 'utf-8', 'ignore')
		if self.filetag == True:
			codecs.open(self.filepath+'\\'+self.filename, 'w', 'utf-8').write(text)
		else:
			dirS = QFileDialog.getSaveFileName(self, "Save File", self.filepath, \
				"sw Files (*.sw);;Text Files (*.txt)")
			if dirS != "":
				codecs.open(dirS, 'w', 'utf-8').write(text)
				dirS = unicode(dirS.toUtf8(), 'utf-8', 'ignore')
				self.filepath, self.filename = os.path.split(dirS)
				self.filetag = True
				self.workPathLabel.setText(self.filepath)

	def closeEvent(self, event):
		cleanfiles()

if __name__ == "__main__":
	app = QApplication(sys.argv)
	QtGui.qApp.setStyleSheet(themeQss)
	window = SWCompiler()
	sys.exit(app.exec_())