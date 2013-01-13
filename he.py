# -*- coding: utf-8 -*-
import os
import sys
import re
from datetime import datetime
p='' #"E:\\sim-icq\\simsim\\sim-im\\sim-td"				#path
try:
	p = sys.argv[1]
	if help in p:
		print """Replace header-template is header.tmpl
			CALLING:
			python -i he.py
			start()"""
except:
	p="."
	
h=p+os.sep + "header.tmpl"		#headerfile to set	

def checkFileTypes(ft, fs):
		return reduce(lambda x, y: x or y, [i in fs for i in ft], 0)

blocked=["moc_", "ui_", "cmake", ".htm", ".html", "openssl", "gtest", "gmock" ] #additional blocked files/directories
def f(x):
	global blocked #=["moc_", "ui_", "cmake", ".html", "openssl", "gtest", "gmock" ]
	found=False
	for block in blocked:
		found |= block in x
	return not found		
		
def start():
	global h
	global p
	global blocked
	rootdir=p
	fileList = []
	ft=[".cpp", ".h"]
	for root, subFolders, files in os.walk(rootdir):
		if checkFileTypes(blocked, root): #prefilter dirs
			continue
		files=filter(f, files) #prefilter for moc and ui generated files. These files don't need to be processed.
		for fi in files:
			if checkFileTypes(ft, fi):
				fileList.append(os.path.join(root,fi))
	#print fileList
	out=open("files.txt","w")
	for fi in fileList:
		out.write(fi+"\n")
	out.close()
	hf=open(h,'r') #reading headerfile
	h_data=hf.read()
	h_data=h_data.replace('<year>'		, str(datetime.now().year)) #....
	h_data=h_data.replace('<date>'		, datetime.now().ctime())
	hf.close()
	SCANNER = re.compile(r'''
		(\s+) |                      # whitespace
		(//)[^\n]* |                 # comments
		(/\*.*\*/)  |				  # mlcomments
		0[xX]([0-9A-Fa-f]+) |        # hexadecimal integer literals
		(\d+) |                      # integer literals
		(<<|>>) |                    # multi-char punctuation
		([][(){}<>=,;:*+-/]) |       # punctuation
		([A-Za-z_][A-Za-z0-9_]*) |   # identifiers
		"""(.*?)""" |                # multi-line string literal
		"((?:[^"\n\\]|\\.)*)" |      # regular string literal
		(.)                          # an error!
	''', re.DOTALL | re.VERBOSE)
	for i in fileList:
		fi=open(i,'r')
		data=fi.read()
		fi.close()
		for match in re.finditer(SCANNER, data):
			space, comment, mlcomment, hexint, integer, mpunct, \
			punct, word, mstringlit, stringlit, badchar = match.groups()
			#if space: ...
			#if comment: ...
			# ... 
			#if badchar: raise FooException...
			#if comment:
			#	print comment
			if mlcomment and h:
				print mlcomment
				fi=open(i,'w')
				fi.write(data.replace(mlcomment, h_data.replace('<filename>'	, i[i.rfind(os.sep)+1:] ) ))
				fi.close()
		if "about.cpp" in i: #testcase
			return
#start()

