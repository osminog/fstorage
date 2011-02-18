#!/usr/bin/env python
# -*- coding: utf-8 -*-

from pyfstorage import Fstorage

fs = Fstorage("fstp://127.0.0.1:8001")

try:
	fs.file_send("file key", "./example1.py")
except:
	print "file  exist ?"

fs.file_recv("file key", "./example1.py.recv")
fs.file_remove("file key")

