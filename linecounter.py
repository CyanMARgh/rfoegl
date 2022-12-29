#!/usr/bin/python3

import os

def find_files(whitelist, blacklist, path):
	walker = next(os.walk(path))
	dirs, files = walker[1], walker[2]
	ans = 0
	for f in files:
		if(f.split('.')[-1] in whitelist and f[0] != '.'):
			print(f)
			F = open(path + f, 'r', encoding="utf8", errors='ignore')
			for line in F:
				ans += 1
	for d in dirs:
		fullpath = path + d + "/"
		if d not in blacklist:
			ans += find_files(whitelist, blacklist, fullpath)
	return ans

result = find_files({'FLAGS', 'h', 'c', 'cpp', 'frag', 'vert', 'comp', 'geom'}, {'build', 'tmp', '.git'}, "./")
print("result = " + str(result))