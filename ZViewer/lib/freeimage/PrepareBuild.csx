import os
import glob
import sys
import shutil

def copy(src_dir, platform, config):
  src_file_names = src_dir + "/" + platform + "/" + config + "/*.*"
  dest_dir = src_dir + "/" + "../../output"
  print("src : " + src_file_names)
  files = glob.glob(src_file_names)

  for file in files:
    if file.lower().endswith(".dll"):
      print(file + " to " + dest_dir)
      shutil.copy(file, dest_dir)

def printUsage():
  print("Usage " + sys.argv[0] + " Win32|x64 Debug|Release")

def main():
  if len(sys.argv) == 3:
    platform = sys.argv[1]
    config = sys.argv[2]

    if platform in {"Win32", "x64"} and config in {"Debug", "Release"}:
      src_dir = os.path.abspath(os.path.dirname(sys.argv[0]))
      
      copy(src_dir, platform, config)
      return
  printUsage()
  exit(1)

if __name__ == "__main__":
  main()
