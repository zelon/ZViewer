"""
 이 파일은 각 언어에 맞는 언어 파일을 테스트하는 파일입니다.
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

	for file in collectFilesList:
		f = open(file, "r")
		data = f.read()
		f.close()

		regstr = r"GetMessage[^\{\},\"]*?\"(.*?)\""

		result = re.findall(regstr, data)

		if len(result) > 0:
			#print file
			#print result

			for key in result:
				collectedUsedKeyList[key] = 1

	for key in collectedUsedKeyList:
		if len(key) > 0:
			#print "^" + key + "$"

			bFound = False

			for lang in langMap:
				if key == lang:
					bFound = True
					break


			if bFound == False:
				print "[FAILED]Not found in all lang : " + key
				return False

	for lang in langMap:
		if False == (lang in collectedUsedKeyList):
			print "[WARNING] Not found in source code : " + lang


def checkSameMap():
	files = glob.glob("output/language/*.txt")

	""" 모든 텍스트 파일을 열면서 key(=왼쪽부분) 의 모든 종류를 모으고
		다시 모든 파일을 돌면서 key 가 없는 파일을 찾음 """


	#print "Collecting language map..."
	for file in files:

		#print "Open " + file
		f = open(file)

		datas = unicode(f.read(), "UTF-16", "ignore")
		f.close()

		lines = datas.split("\n")
		for line in lines:
			line = line.strip()
			if len(line) <= 0:
				continue
			#print "^" + line + "|"

			# line 에 = 가 있는지 체크해야함
			sline = line.split("=")

			langMap[sline[0]] = sline[1]

	#print "Test files..."

	bFound = False
	for file in files:
		#print "Open " + file
		f = open(file)

		datas = unicode(f.read(), "UTF-16", "ignore")
		f.close()

		thisLangMap = {}
		lines = datas.split("\n")
		for line in lines:
			line = line.strip()
			if len(line) <= 0:
				continue
			#print "^" + line + "$"

			# line 에 = 가 있는지 체크해야함
			sline = line.split("=")

			thisLangMap[sline[0]] = sline[1]

		for thisLang in langMap:
			try:
				test = "find : " + thisLangMap[thisLang]
				#print test
				pass
			except:
				bFound = True
				print "there is no matching word for : " + thisLang + " in " + file

	if False == bFound:
		print "[OK] There is no error for checking same word."

""" Resource 파일들이 서로 같은지 체크~~~ """
def checkSameRes():
	left = open(r'ZViewer\res\resource.h', "r")
	right = open(r'LangKor\res\resource.h', "r")

	leftdata = left.read()
	left.close()

	rightdata = right.read()
	right.close()

	#print leftdata
	reDefines = r"#define.*"
	leftdefines = re.findall(reDefines, leftdata)
	rightdefines = re.findall(reDefines, rightdata)

	lcount = len(leftdefines)
	rcount = len(rightdefines)

	if lcount == rcount:
		print "[OK] Define count same"
	else:
		print "[FAILED] !!! Define count not same"
		return False

	reNextValue = r"_APS_NEXT_COMMAND_VALUE.*?(\d+)"
	lnextValue = re.findall(reNextValue, leftdata)
	rnextValue = re.findall(reNextValue, rightdata)

	if len(lnextValue) == len(rnextValue) and lnextValue[0] == rnextValue[0]:
		print "[OK] Same next value : " + lnextValue[0]
	else:
		print "[FAILED] Not same next value!! left:" + lnextValue[0] + " and right:" + rnextValue[0]
		return False

	reDefineValues = r"#define.*?(\d+)"
	lDefineValues= re.findall(reDefineValues, leftdata)
	rDefineValues= re.findall(reDefineValues, rightdata)

	for l in lDefineValues:
		if int(l) > int(lnextValue[0]):
			print "[FAILED] over next value : " + l
			print "[FAILED] next value : " + lnextValue[0]
			return False

	for r in lDefineValues:
		if int(r) > int(rnextValue[0]):
			print "[FAILED] over next value : " + r
			print "[FAILED] next value : " + rnextValue[0]
			return False

if __name__ == "__main__":
	print "---- start checkSameMap ----"
	if False == checkSameMap():
		exit(1)
	print "---- start checkUseMap ----"
	if False == checkUsedMap():
		exit(1)
	print "---- start checkSameResource ----"
	if False == checkSameRes():
		exit(1)
	exit(0)
