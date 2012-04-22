#####################################################################################
## Copyright (c) 2012, pa.eeapai@gmail.com                                         ##
## All rights reserved.                                                            ##
##                                                                                 ##
## Redistribution and use in source and binary forms, with or without              ##
## modification, are permitted provided that the following conditions are met:     ##
##     * Redistributions of source code must retain the above copyright            ##
##       notice, this list of conditions and the following disclaimer.             ##
##     * Redistributions in binary form must reproduce the above copyright         ##
##       notice, this list of conditions and the following disclaimer in the       ##
##       documentation and/or other materials provided with the distribution.      ##
##     * Neither the name of the pa.eeapai@gmail.com nor the                       ##
##       names of its contributors may be used to endorse or promote products      ##
##       derived from this software without specific prior written permission.     ##
##                                                                                 ##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ##
## ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   ##
## WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          ##
## DISCLAIMED. IN NO EVENT SHALL pa.eeapai@gmail.com BE LIABLE FOR ANY             ##
## DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES      ##
## (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;    ##
## LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND     ##
## ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT      ##
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS   ##
## SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                    ##
#####################################################################################

from zipfile import ZipFile
import FileUtils as fu
import os


def shouldUnzipPath(pathInZip, zipPath):
  return zipPath.startswith(pathInZip)

def getPathToUnzip(pathToUnzipRoot, pathInZip, zipPath):    
  pathFromZip = fu.convertToBackslashes(zipPath[len(pathInZip):])
  if '\\' == pathFromZip[0]:
    pathFromZip = pathFromZip[1:]
  return pathToUnzipRoot + pathFromZip 

def extract(pathToZip, pathInZip, pathToUnzip):
  if not pathToUnzip.endswith('\\'):
    pathToUnzip = pathToUnzip + '\\'
  print "Extracting {0} from {1} to {2}".format(pathInZip, pathToZip, pathToUnzip)
  try:
    z = ZipFile(pathToZip)
    for name in z.namelist():
      if not shouldUnzipPath(pathInZip, name):
        continue    
      extractDestinationPath = getPathToUnzip(pathToUnzip, pathInZip, name)
      if name.endswith('/'):
        if not fu.folderExists(extractDestinationPath):
          os.makedirs(extractDestinationPath)
      else:
        print name + " -> " + extractDestinationPath
        with open(extractDestinationPath, 'wb') as destinationFile:       
          destinationFile.write(z.read(name))
    print 'SUCCESS'
  except:
    print 'FAIL'
