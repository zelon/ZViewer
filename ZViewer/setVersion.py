import re
import os

def commonDefineVersionUp(major, minor, patch, postfix):
	print ("CommonDefine.h file update...")

	filename = r"lib\CommonDefine.h"
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

	return True

def zviewerAgentRcVersionUp(major, minor, patch):
	print ("ZViewAgent rc file Version update...")

	file = open(r"ZViewerAgent\ZViewerAgent.rc")
	
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
	
	file = open(r"ZViewerAgent\ZViewerAgent.rc", "w")
	file.writelines(outputs)
	file.close()

def zviewerRcVersionUp(major, minor, patch):
	print ("ZViewer rc file Version update...")

	file = open(r"ZViewer\res\ZViewer.rc")
	
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

	file = open(r"ZViewer\res\ZViewer.rc", "w")
	file.writelines(outputs)
	file.close()

def nsisVersionUp(major, minor, patch, postfix):
    print ("nsis file Version update...")
    
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
    

def doVersionUp():

	print("Enter Major : "),
	vMajor = input()

	print("Enter Minor : "),
	vMinor = input()

	print("Enter patch : "),
	vPatch = input()

	print("Enter postfix : "),
	vPostfix = raw_input()

	# version up ZViewerAgent\ZViewerAgent.rc
	if False == zviewerRcVersionUp(vMajor, vMinor, vPatch):
		print ("Error on zviewerRcVersionUp!!!!!!!!!!")
		return

	# version up ZViewer/res/ZViewer.rc
	if False == zviewerAgentRcVersionUp(vMajor, vMinor, vPatch):
		print ("Error on zviewerAgentRcVersionUp!!!!!!!!!!")
		return

	# version up output/ZViewer.nsi
	if False == nsisVersionUp(vMajor, vMinor, vPatch, vPostfix):
		print ("Error on nsisVersionUp!!!!!!!!!!")
		return

	# version up ZViewer/CommonDefin.h
	if False == commonDefineVersionUp(vMajor, vMinor, vPatch, vPostfix):
		print ("Error on commonDefineVersionUp!!!!!!!!!!")
		return

	print("version update completed")

if __name__ == "__main__":
	doVersionUp()
	os.system("pause")
