#!/usr/bin/env python

import re
import sys
import base64

def relpath(base, rel):
    return '/'.join(base.split('/')[:-1]) + '/' + rel

def processPic(path):
    f = open(path)
    content = f.read()
    f.close()
    ext = re.search('[^.]*$', path).group(0)
    return "data:image/" + ext + ";base64," + base64.b64encode(content)

def processJs(path):
    f = open(path)
    content = f.read()
    f.close()
    return content

def processCss(path):
    f = open(path)
    content = f.read()
    f.close()

    #process all pic
    pattern = re.compile(r'url\(([^)]*\.(png|gif))\)')
    mo = pattern.search(content)
    while mo != None:
        content = content[0:mo.start()] +\
                'url(' +\
                processPic(relpath(path, mo.group(1))) +\
                ')' +\
                content[mo.end():]
        mo = pattern.search(content)

    return content

def processHtml(path):
    f = open(path)
    content = f.read()
    f.close()

    #process all css
    pattern = re.compile(r'<link rel="stylesheet" href="(.*)" type="text/css">')
    mo = pattern.search(content)
    while mo != None:
        content = content[0:mo.start()] +\
            '<style type="text/css">' +\
            processCss(relpath(path, mo.group(1))) +\
            '</style>' +\
            content[mo.end():]
        mo = pattern.search(content)

    #process all js
    pattern = re.compile(r'<script src="(.*)"></script>')
    mo = pattern.search(content)
    while mo != None:
        content = content[0:mo.start()] +\
            '<script>' +\
            processJs(relpath(path, mo.group(1))) +\
            '</script>' +\
            content[mo.end():]
        mo = pattern.search(content)

    return content

def main():
    if len(sys.argv) != 3:
        print sys.argv[0] + " <input> <output>"
        return
    f = open(sys.argv[2], 'w')
    f.write(processHtml(sys.argv[1]))
    f.close()

if __name__ == '__main__':
    main()
