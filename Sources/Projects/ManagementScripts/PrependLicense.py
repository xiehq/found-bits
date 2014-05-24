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

import datetime
import os
import FileUtils as fu

newBSDLicTemplate = '''Copyright (c) <year>, <copyright holder>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.'''

now = datetime.datetime.now()
year = str(now.year)
copyrightHolder = "pa.eeapai@gmail.com"
organization = "pa.eeapai@gmail.com"

lic = newBSDLicTemplate.replace("<year>", year)
lic = lic.replace("<copyright holder>", copyrightHolder)
lic = lic.replace("<COPYRIGHT HOLDER>", copyrightHolder)
lic = lic.replace("<organization>", organization)

licLines = lic.splitlines()
maxLicLineLength = 0
for line in licLines:
  if len(line) > maxLicLineLength: maxLicLineLength = len(line)

class Lang:
  def __init__(self, lang, exts, comment):
    self.lang = lang
    self.exts = exts
    self.comment = comment

languages = [Lang("Python"    , ["py"]            , "## ##"),
             Lang("C/C++"     , ["c", "cpp", "h"] , "// //"),
             Lang("C#"        , ["cs"]            , "// //"),
             Lang("GCC Linker", ["ind"]           , "/* */"),
             Lang("ASM"       , ["s"]             , "## ##")
             ]

def hasLicense(data):
  if -1 == data.find(licLines[1], 0, len(lic) * 2): return False
  if -1 == data.find(licLines[5], 0, len(lic) * 2): return False
  if -1 == data.find(licLines[7], 0, len(lic) * 2): return False
  return True

def prependLicense(path, comment, data):
  with file(path, 'w') as modified:
    commentAtTheEnd = not comment.endswith(" ")
    commentExtender = comment[0]
    additionalFillerLength = 3
    if commentAtTheEnd:
      commentExtender = comment[1]
      additionalFillerLength = 2
    
    modified.write(comment.replace(" ", (maxLicLineLength + additionalFillerLength) * commentExtender) + "\n")
    for licLine in licLines:
      line = ""
      if commentAtTheEnd:
        line = comment.replace(" ", " " + licLine + " " * (maxLicLineLength + 1 - len(licLine))) + "\n"
      else:
        line = comment.replace(" ", " " + licLine + " " * (maxLicLineLength + 1 - len(licLine))) + commentExtender + "\n"
      modified.write(line)
    modified.write(comment.replace(" ", (maxLicLineLength + additionalFillerLength) * commentExtender) + "\n" + "\n")
    modified.write(data)
  
def addLicense(path):
  ans = raw_input("Add license to \"" + path + "\" [ENTER for NO, anything for YES]:")
  if len(ans):
    return True
  return False

def checkForLicense(path, comment):
  with file(path, 'r') as original:
    data = original.read()
    if hasLicense(data):
      print path + " has a license"
      return    
    if addLicense(path):
      print "Adding license to: " + path
      backupPath = path + ".backup"
      fu.copy(path, backupPath) 
      prependLicense(path, comment, data)
      fu.delete(backupPath)

def checkLicenses(path):
  for root, dirs, files in os.walk(path):
    for file in files:
      filePath = os.path.join(root, file)
      if not -1 == filePath.find("\\."):
        continue 
      for lang in languages:
        for ext in lang.exts:
          if filePath.endswith("." + ext):
            checkForLicense(filePath, lang.comment)
      
      
#checkLicenses("\\Sources\\Common\\")  
#checkLicenses("\\Sources\\LinkerScripts\\")
#checkLicenses("\\Sources\\Projects\\")
checkLicenses("\\Sources\\Projects\\Libs\\")
  
  
  
  
  
  
  
  
  