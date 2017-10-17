import sys, os, time
import json
import threading
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from PyQt4 import uic
from utils import Instruction, Interpret

UI_MainWindow, QtBaseClass = uic.loadUiType("swCompiler.ui")
UI_RtmWindow, RuntimeClass = uic.loadUiType("runWindow.ui")

def cleanfiles():
	from os.path import isfile
	if isfile(".\\~.tmp"):
		os.remove(".\\~.tmp")
	if isfile(os.getcwd()+"\\ferr.json"):
		os.remove(os.getcwd()+"\\ferr.json")
	if isfile(os.getcwd()+"\\fcode.json"):
		os.remove(os.getcwd()+"\\fcode.json")

def judge():
	from os.path import isfile
	startTime = time.time()
	while True:
		if isfile(os.getcwd()+"\\ferr.json"):
			return True
		if time.time()-startTime > 3:
			return False
		time.sleep(0.1)

def procRun(codeList, window):
	window.interpret = Interpret(codeList)
	run = window.interpret
	while True:
		print run.showStack()
		tag = run.sg_step()
		#print run.showStack()
		if tag == 1:	#input
			window.input()
		if tag == 2:	#output
			window.output(run.recv())
		if run.judge() == False:
			break
	window.output("=== the processing is over ===")

class RuntimeWin(QMainWindow, UI_RtmWindow):
	def __init__(self, mod = 0):
		QMainWindow.__init__(self)
		UI_RtmWindow.__init__(self)
		self.setupUi(self)
		self.mod = mod
		self.iptsgl = 0
		self.initUI()

	def output(self, s):
		if type(s) == type(1):
			s = str(s)
		self.RuntimePad.textCursor().insertText(s+'\n')

	def input(self):
		self.RuntimePad.textCursor().insertText(">>>\t")
		self.InputTextPad.setEnabled(True)
		ipt = None
		while self.iptsgl == 0:
			time.sleep(0.05)
		self.iptsgl = 0
		ipt = self.InputTextPad.text()
		self.interpret.send(ipt.toInt()[0])
		self.RuntimePad.textCursor().insertText(ipt+"\n")
		self.InputTextPad.setText("")
		self.InputTextPad.setEnabled(False)

	def inputwait(self):
		self.iptsgl = 1

	def initUI(self):
		self.InputTextPad.setEnabled(False)
		self.connect(self.InputTextPad, SIGNAL("returnPressed()"), self.inputwait)

		if self.mod == 0:
			self.actionStepnext.setEnabled(False)
			self.actionStepinto.setEnabled(False)
			self.actionStepover.setEnabled(False)
			self.actionStepout.setEnabled(False)

		import codecs
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
		cleanfiles()

class SWCompiler(QMainWindow, UI_MainWindow):
	def __init__(self):
		QMainWindow.__init__(self)
		UI_MainWindow.__init__(self)
		self.setupUi(self)
		self.initUI()

	def errTbInit(self):
		self.errorMsgTable.clear()
		self.errorMsgTable.setColumnCount(3)
		self.errorMsgTable.setRowCount(1)
		self.errorMsgTable.setHorizontalHeaderLabels(['errno', 'line', 'message'])
		self.errorMsgTable.verticalHeader().setVisible(False)
		self.errorMsgTable.setEditTriggers(QAbstractItemView.NoEditTriggers)
		self.errorMsgTable.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.errorMsgTable.setColumnWidth(0, 80)
		self.errorMsgTable.setColumnWidth(2, 550)

	def fileInit(self):
		self.filetag = False
		self.filepath = os.getcwd()
		self.filename = ""
		cleanfiles()

	def initUI(self):
		self.fileInit()
		self.errTbInit()
		self.actionNew.triggered.connect(self.newFile)
		self.actionOpen.triggered.connect(self.openFile)
		self.actionSave.triggered.connect(self.saveFile)
		self.actionBuildAndRun.triggered.connect(self.BuildAndRun)
		
		self.show()

	def BuildAndRun(self):
		import codecs
		text = self.codeTextEdit.toPlainText()
		text = unicode(text.toUtf8(), 'utf-8', 'ignore')
		if text == "":
			text = u" "
		curfile = self.filepath+'\\'
		if self.filetag == True:
			curfile = curfile+self.filename
		else:
			curfile = curfile+"~.tmp"

		codecs.open(curfile, 'w', 'utf-8').write(text)
		os.system(os.getcwd()+"\\swcpl.exe "+curfile)
		if judge() == True:
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
				headItem = self.errorMsgTable.horizontalHeaderItem(idx)
				idx += 1
				
			if errData[u'errNum'] == 0:
				self.runDlg = RuntimeWin(0)
				self.IOthread = threading.Thread(target=procRun,\
					args=(self.runDlg.codeList, self.runDlg))
				#self.IOthread.setDaemon('True')
				self.IOthread.start()
				cleanfiles()
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
				import codecs
				text = codecs.open(dirO, 'r', 'utf-8').read()
				self.codeTextEdit.setPlainText(text)

				dirO = unicode(dirO.toUtf8(), 'utf-8', 'ignore')
				self.filepath, self.filename = os.path.split(dirO)
				self.filetag = True

	def saveFile(self):
		import codecs
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

	def closeEvent(self, event):
		cleanfiles()

if __name__ == "__main__":
	app = QApplication(sys.argv)
	window = SWCompiler()
	sys.exit(app.exec_())