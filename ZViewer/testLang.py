"""
 This file tests all kind of language file and resource file
"""
import os
import re
import glob

langMap = {}
collectFilesList = []
includeExts = [ ".c", ".cpp", ".h", ".hpp" ]

collectedUsedKeyList = {}

def collectFiles(dir):
	files = glob.glob(dir + r"\*")

	for file in files:
		if os.path.isfile(file):

			for ext in includeExts:
				if file.endswith(ext):
					collectFilesList.append(file)

		if os.path.isdir(file):
			collectFiles(file)


def checkUsedMap():
	collectFiles(".")

	collectedUsedKeyList.clear()

	bFailed = False
	for file in collectFilesList:
		#print("File : " + file)
		f = open(file, "rb")

		try:
			data = f.read().decode("utf-8")
		except UnicodeDecodeError:
			print("[FAILED] UnicodeError(not utf-8) : " + file)
			bFailed = True
			continue
		f.close()

		regstr = r"GetMessage[^\{\},\"]*?\"(.*?)\""

		result = re.findall(regstr, data)

		if len(result) > 0:
			#print(file)
			#print(result)

			for key in result:
				collectedUsedKeyList[key] = 1

	if bFailed:
		return False
		
	for key in collectedUsedKeyList:
		if len(key) > 0:
			#print("^" + key + "$")

			bFound = False

			for lang in langMap:
				if key == lang:
					bFound = True
					break


			if bFound == False:
				print("[FAILED]testLang : error lang:Not found in all lang : " + key)
				return False

	for lang in langMap:
		if False == (lang in collectedUsedKeyList):
			print("[WARNING]testLang : warning lang:Not found in source code : " + lang)


def checkSameMap():
	files = glob.glob("output/language/*.txt")

	""" Check every file has same kind of key and value.
	    check odd key and some language file has that key """


	#print("Collecting language map...")
	for file in files:

		#print("Open " + file)
		f = open(file, "rb")

		datas = f.read().decode("UTF-16")
		f.close()

		lines = datas.split("\n")
		for line in lines:
			line = line.strip()
			if len(line) <= 0:
				continue
			#print("^" + line + "|")

			# '=' should exist in a line
			sline = line.split("=")

			langMap[sline[0]] = sline[1]

	#print("Test files...")

	bFound = False
	for file in files:
		#print("Open " + file)
		f = open(file, "rb")

		datas = f.read().decode("UTF-16")
		f.close()

		thisLangMap = {}
		lines = datas.split("\n")
		for line in lines:
			line = line.strip()
			if len(line) <= 0:
				continue
			#print("^" + line + "$")

			# '=' should exist in a line
			sline = line.split("=")

			thisLangMap[sline[0]] = sline[1]

		for thisLang in langMap:
			try:
				test = "find : " + thisLangMap[thisLang]
				#print(test)
				pass
			except:
				bFound = True
				print(file + ": error lang:there is no matching word for : " + thisLang + " in " + file)
				print("[FAILED]there is no matching word for : " + thisLang + " in " + file)

	if False == bFound:
		print("[OK] There is no error for checking same word.")

def checkNSISLanguageFiles():
	strLangDir = "output/language/"
	files = [ strLangDir + "english.nsh", strLangDir + "korean.nsh" ]

	strRe = r"^LangString[ \t]+.*?${CURLANG}[ \t]+\""
	strRe = r"^LangString[ \t]+.*?\${CURLANG}[ \t]+\""
	if False == checkSamePattern(files, strRe):
		return False
	print("[OK] All NSIS language files are checked.")
	return True

def checkSamePattern(files, strRe, printResult = False):
	if len(files) <= 1:
		print("testLang.py: error test lang:no or just one file list")
		print("[FAILED]no or just one file list")
		return False

	file = open(files[0], "r")

	data = file.read()
	file.close()

	compareFlag = re.S | re.I | re.M 
	result = re.findall(strRe, data,  compareFlag)

	if True == printResult:
		print(result)

	for file in files:
		hFile = open(file, "r")
		compareData = hFile.read()
		hFile.close()

		compareResult = re.findall(strRe, compareData, compareFlag)

		if result != compareResult:
			print("---------------------------------------------")
			print(files[0] + ":")
			print(result)
			print("---------------------------------------------")
			print(file + ":")
			print(compareResult)
			print("---------------------------------------------")
			print("testLang.py: error test lang:Compare failed " + file + " with " + files[0])
			print("[FAILED] Compare failed " + file + " with " + files[0])
			return False

	return True

""" Check if resource.h files are same """
def checkSameRes():
	left = open(r'ZViewer\res\resource.h', "r")
	right = open(r'LangKor\res\resource.h', "r")

	leftdata = left.read()
	left.close()

	rightdata = right.read()
	right.close()

	#print(leftdata)
	reDefines = r"#define.*"
	leftdefines = re.findall(reDefines, leftdata)
	rightdefines = re.findall(reDefines, rightdata)

	lcount = len(leftdefines)
	rcount = len(rightdefines)

	if lcount == rcount:
		print("[OK] Define count same")
	else:
		print("[FAILED]testLang : error lang:  Define count not same")
		return False

	reNextValue = r"_APS_NEXT_COMMAND_VALUE.*?(\d+)"
	lnextValue = re.findall(reNextValue, leftdata)
	rnextValue = re.findall(reNextValue, rightdata)

	if len(lnextValue) == len(rnextValue) and lnextValue[0] == rnextValue[0]:
		print("[OK] Same next value : " + lnextValue[0])
	else:
		print("[FAILED]testLang : error lang: Not same next value!! left:" + lnextValue[0] + " and right:" + rnextValue[0])
		return False

	reDefineValues = r"#define.*?(\d+)"
	lDefineValues= re.findall(reDefineValues, leftdata)
	rDefineValues= re.findall(reDefineValues, rightdata)

	for l in lDefineValues:
		if int(l) > int(lnextValue[0]):
			print("[FAILED]testLang : error lang: over next value : " + l)
			print("[FAILED]testLang : error lang: next value : " + lnextValue[0])
			return False

	for r in lDefineValues:
		if int(r) > int(rnextValue[0]):
			print("[FAILED]testLang : error lang: over next value : " + r)
			print("[FAILED]testLang : error lang: next value : " + rnextValue[0])
			return False

	if leftdata != rightdata:
		print("[FAILED]testLang : error lang: Not same resource.h")
		return False

	print("[OK] Completed checking resource.h")

if __name__ == "__main__":
	if False == checkSameMap():
		exit(1)
	if False == checkUsedMap():
		exit(1)
	if False == checkSameRes():
		exit(1)
	if False == checkNSISLanguageFiles():
		exit(1)
	exit(0)
