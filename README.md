
# Introduction
 * ZViewer is a Image Viewer on Microsoft Windows System.
 * ZViewerAgent is a explorer extension to preview image files in explorer.
 * Official blog is http://zviewer.wimy.com
  
# Main Features
 
  - Cache next image
  - Paste clipboard image as file ( in explorer )
  - Support Unicode
  - Support Preview in explorer ( with right click )
  - Support various image format : bmp, jpg, jpeg2000, png, tif, psd, gif, dds, tga, pcx, xpm, xbm, cut, ico, hdr, jng, koa, mng, pcd, ras, wbmp
  - Auto Rotate

# Build Environment
 * Use vcpkg for freeimage library
   * .\vcpkg.exe install freeimage:x64-windows freeimage:x64-windows-static

# How to run

## ZViewer
 * Build
 * Run ZViewer.exe

## ZViewerAgent
 * Build
 * "regsvr32 /s /c ZViewerAgent.dll" on cmd.exe as Administrator
 * Right click on some image files in explorer
 * Deregister: "regsvr32 /u ZViewerAgent.dll" on cmd.exe as Administrator
