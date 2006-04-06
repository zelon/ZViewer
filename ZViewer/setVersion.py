import re

def commonDefineVersionUp(major, minor, patch):
	print ("CommonDefine.h ...")

	file = open(r"ZViewer\CommonDefine.h")

	lines = file.readlines()
	
	outputs = []	
	for i in lines:

		if re.match(r"const std::string g_strVersion = \"\d.\d.\d\"", i) != None:
			outputs.append("const std::string g_strVersion = \"" + str(major) + "." + str(minor) + "." + str(patch) + "\";\n")
		else:
			outputs.append(i)

	file.close()

	file = open(r"ZViewer\CommonDefine.h", "w")
	file.writelines(outputs)
	file.close()

def zviewerAgentRcVersionUp(major, minor, patch):
	print ("rcVersion...")

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
	print ("rcVersion...")

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

def nsisVersionUp(major, minor, patch):
    print ("nsisVersion...")
    
    file = open(r"output\ZViewer.nsi")
    
    lines = file.readlines()
    
    outputs = []    
    for i in lines:
    	
    	m = re.match(r"!define\sPRODUCT_VERSION\s(.*)\n", i)
    	
    	if m != None:
    		outputs.append("!define PRODUCT_VERSION \"" + str(major) + "." + str(minor) + "." + str(patch) + "\"\n")
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

	# version up ZViewerAgent\ZViewerAgent.rc
	zviewerRcVersionUp(vMajor, vMinor, vPatch)

	# version up ZViewer/res/ZViewer.rc
	zviewerAgentRcVersionUp(vMajor, vMinor, vPatch)

	# version up output/ZViewer.nsi
	nsisVersionUp(vMajor, vMinor, vPatch)

	# version up ZViewer/CommonDefin.h
	commonDefineVersionUp(vMajor, vMinor, vPatch)

if __name__ == "__main__":
    doVersionUp()