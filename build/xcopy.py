#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os
import shutil #for file rm and copy
import fnmatch

class xcopy:
    def __init__(self, src, dst, filters=[]):
        self.filters = filters
        self.copytree(src, dst)
    def copytree(self, src, dst):
        names=os.listdir(src)
        if not os.path.isdir(dst):
            os.makedirs(dst)
        errors=[]
        for name in names:
            srcname=os.path.join(src,name)
            dstname=os.path.join(dst,name)
            if os.path.isdir(srcname):
                self.copytree(srcname, dstname)
            elif os.path.isfile(srcname):
                if self.filterName(name):
                    print("copy:"+name+" "+dstname)
                    shutil.copy2(srcname, dstname)
        shutil.copystat(src, dst)

    def filterName(self, fileName):
        for filter in self.filters:
            if fnmatch.fnmatch(fileName, filter):
                return True
        return False