
using System;
using System.IO;

class CopyLibrary
{
  public static void Copy(string lib_dir, string platform, string config)
  {
    string src_dir = lib_dir + "\\" + platform + "\\" + config;
    string dest_dir = lib_dir + "\\" + "..\\..\\output";
    foreach (string src_filename in Directory.EnumerateFiles(src_dir)) {
      string dest_filename = dest_dir + "\\" + Path.GetFileName(src_filename);
      Console.WriteLine("Copying " + src_filename + " to " + dest_filename);
      File.Copy(src_filename, dest_filename, /*overwrite=*/true);
    }
  }

  public static void PrintUsage()
  {
    System.Console.WriteLine("Usage: csi.exe PrepareBuild.csx Win32|x64 Debug|Release");
  }

  public static void Main()
  {
    string[] argv = System.Environment.GetCommandLineArgs();
    if (argv.Length != 4)
    {
      PrintUsage();
      return;
    }

    string platform = argv[2];
    string config = argv[3];
    if (platform != "Win32" && platform != "x64")
    {
      PrintUsage();
      return;
    }
    if (config != "Debug" && config != "Release")
    {
      PrintUsage();
      return;
    }

    string current_script_filename = argv[1];
    string src_dir = Path.GetDirectoryName(Path.GetFullPath(current_script_filename));
    Copy(src_dir, platform, config);
  }
}

CopyLibrary.Main();
