# -*- coding: utf-8 -*-

"""
Copyright (c) 2009-2010 by Fstorage Team.
Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
"""

from distutils.core import setup, Extension

setup(
    name = 'pyfstorage',
    version = "0.3",
    description = 'File Storage',
    author = 'Lukasz Marcin Podkalicki',
    author_email = 'lukasz@fstorage.pl',
    license = "BSD License",
    url = 'http://fstorage.pl/', # dodać ścieżkę do pobrania
    package_data = {"": ["LICENSE", "README"]},
    ext_modules = [
	    Extension('pyfstorage', ['fstorage_client_python.c'], libraries=['fstorage'])
    ]
)
