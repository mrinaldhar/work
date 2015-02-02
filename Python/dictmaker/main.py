import re
import os
def get_data(file, char):
	file.seek(0)
	array = []
	array = file.read().split(char)
	array.sort()
	return array
def put_data(array, file):
	file.seek(0)
	for each in array:
		file.write(each+'\n')
def merge(array1, array2):
	for each in array2:
		if check(array2, each) is True and '-' not in each and len(each) != 1:
			if each.lower() not in array1:
				array1.append(each.lower())
	array1.sort()
	return array1
def check(arraysrc, word):
	array = re.findall('[^A-Za-z]+', word)
	if (len(array)==0):
		return True
	else:
#		resolve(word, arraysrc)
	 	return False
#def resolve(word, array):
#ar = re.findall('[A-Za-z]+', word)
#	for each in ar:
#		array.append(each)


fn1 = raw_input("Enter the dictionary file's name: ")
dn2 = raw_input("Enter the source directory's name: ")
print "Please wait..."
dlist = {}
for each in os.walk(dn2):
	dir = each[0]
	files = each[2]
	dlist[dir] = files
for each in dlist:
	print "Processing files from directory %r" % each
	dname = each
	for item in dlist[each]:
		if item != fn1:
			fn2 = dname + '/' + item
			print "\tReading %r" % item
			try:
				file1 = open(fn1)
				a=get_data(file1, '\n')
			except IOError:
				file1 = open(fn1, 'w')
				a=[]
			file2 = open(fn2)
			b=[]
			b=get_data(file2, ' ')
			a=merge(a, b)
			file1 = open(fn1, 'w')
			put_data(a, file1)
print "All done!"
