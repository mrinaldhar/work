file = open('testfile.mkv', 'r')
i=0
data = file.read(500)
while (data!=''):
	file2 = open(str(i), 'w')
	file2.write(data)
	file2.close()
	i=i+1
	data = file.read(500)
