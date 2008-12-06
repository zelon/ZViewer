import re
import os

def commonDefineVersionUp(major, minor, patch, postfix):

	filename = r"commonSrc\CommonDefine.h"
	if False == os.access(filename, os.F_OK):
		print ("no exist of " + filename)
		return False

	file = open(filename, "r")
	lines = file.readlines()
	file.close()
	
	outputs = []

	bFound = False
	for i in lines:

		if re.match(r"const tstring g_strVersion = TEXT\(\"\d.\d.\d", i) != None:
			outputs.append("const tstring g_strVersion = TEXT(\"" + str(major) + "." + str(minor) + "." + str(patch) + str(postfix) + "\");\n")
			bFound = True
		else:
			outputs.append(i)

	if bFound == False:
		print ("Can't find replace target string")
		return False
	file = open(filename, "w")
	file.writelines(outputs)
	file.close()
	print ("[OK] CommonDefine.h file updated")

	return True

def SetRcVersionUp(rcFileName, major, minor, patch):

	#file = open(r"ZViewerAgent\ZViewerAgent.rc")
	try:
		file = open(rcFileName)
	except:
		print("[FAILED] Cannot open " + rcFileName)
		return False
	
	lines = file.readlines()
	
	productVersionPattern = r"(\s*VALUE \"ProductVersion\", )\"(\d.\d.\d.\d)\""
	fileVersionPattern = r"(\s*VALUE \"FileVersion\", )\"(\d.\d.\d.\d)\""
	
	outputs = []	
	for i in lines:
		
		if re.match(r" FILEVERSION \d,\d,\d,\d\n", i) != None:
			outputs.append(" FILEVERSION " + str(major) + "," + str(minor) + "," + str(patch) + ",0\n")
		elif re.match(r" PRODUCTVERSION \d,\d,\d,\d\n", i) != None:
			outputs.append(" PRODUCTVERSION " + str(major) + "," + str(minor) + "," + str(patch) + ",0\n")
		elif re.match(productVersionPattern, i) != None:
			outputs.append(re.sub(productVersionPattern, r"\1" + "\"" + str(major) + "." + str(minor) + "." + str(patch) + ".0\"", i))
		elif re.match(fileVersionPattern, i) != None:
			outputs.append(re.sub(fileVersionPattern, r"\1" + "\"" + str(major) + "." + str(minor) + "." + str(patch) + ".0\"", i))
		else:
			outputs.append(i)
	
	file.close()
	
	file = open(rcFileName, "w")
	file.writelines(outputs)
	file.close()
	print ("[OK] " + rcFileName + " file Version updated")
	return True

def nsisVersionUp(major, minor, patch, postfix):
    
    file = open(r"output\ZViewer.nsi")
    
    lines = file.readlines()
    
    outputs = []    
    for i in lines:
    	
    	m = re.match(r"!define\sPRODUCT_VERSION\s(.*)", i)
    	
    	if m != None:
    		outputs.append("!define PRODUCT_VERSION \"" + str(major) + "." + str(minor) + "." + str(patch) + str(postfix) + "\"\n")
    	else:
    		outputs.append(i)

    file.close()
   
    file = open(r"output\ZViewer.nsi", "w")
    file.writelines(outputs)
    file.close()
    print ("[OK] nsis file Version updated")
    
def getCurrentVersionFromNSIS():
	
	file = open(r"output\ZViewer.nsi")
    
	lines = file.readlines()
	file.close()
    
	outputs = []    
	for i in lines:
    	
		m = re.match(r"!define\sPRODUCT_VERSION\s\"(.*)\"", i)
    	
		if m != None:
			"""outputs.append("!define PRODUCT_VERSION \"" + str(major) + "." + str(minor) + "." + str(patch) + str(postfix) + "\"\n")"""
			return (m.group(1))
		else:
			pass
	return "Cannot get current version from nsis"

def doVersionUp():

	print("Major is [A].B.Cxxx");
	print("Enter Major : "),
	vMajor = input()

	print("\nMinor is A.[B].Cxxx");
	print("Enter Minor : "),
	vMinor = input()

	print("\nPatch is A.B.[C]xxx");
	print("Enter patch : "),
	vPatch = input()

	print("\nPostfix is A.B.C[xxx]");
	print("Enter postfix : "),
	vPostfix = raw_input()

	print("New version : " + str(vMajor) + "." + str(vMinor) + "." + str(vPatch) + str(vPostfix))
	print("Is it right? (y/N) : "),
	strYesNo = raw_input()

	if strYesNo != "y":
		print ("Canceled by user input")
		return False

	# version up ZViewerAgent\ZViewerAgent.rc
	if False == SetRcVersionUp(r"ZViewerAgent\ZViewerAgent.rc", vMajor, vMinor, vPatch):
		print ("[FAILED] !!! Error on zviewerRcVersionUp!!!!!!!!!!")
		return False

	# version up ZViewer/res/ZViewer.rc
	if False == SetRcVersionUp(r"ZViewer\res\ZViewer.rc", vMajor, vMinor, vPatch):
		print ("[FAILED] !!! Error on zviewerAgentRcVersionUp!!!!!!!!!!")
		return False

	# version up ZViewer/LangKor/res/ZViewer.rc
	if False == SetRcVersionUp(r"LangKor\res\ZViewer.rc", vMajor, vMinor, vPatch):
		print ("[FAILED] !!! Error on LangKor RcVersionUp!!!!!!!!!!")
		return False

	# version up output/ZViewer.nsi
	if False == nsisVersionUp(vMajor, vMinor, vPatch, vPostfix):
		print ("[FAILED] !!! Error on nsisVersionUp!!!!!!!!!!")
		return False

	# version up commonSrc/CommonDefin.h
	if False == commonDefineVersionUp(vMajor, vMinor, vPatch, vPostfix):
		print ("[FAILED] !!! Error on commonDefineVersionUp!!!!!!!!!!")
		return False

	print("[OK] version update completed")

if __name__ == "__main__":
	print("Current version : " + getCurrentVersionFromNSIS())
	if False == doVersionUp():
		exit(1)
