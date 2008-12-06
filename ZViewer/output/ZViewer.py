import re

def getCurrentVersionFromNSIS():
	
	file = open(r"ZViewer.nsi")
    
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
	return "Unknown"

if __name__ == "__main__":
	print(getCurrentVersionFromNSIS())
