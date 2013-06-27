#!/usr/bin/env python

import os
import glob
import shutil
import string
import subprocess


# call markdown as a subprocess, and capture the output
def markdown(raw_file):
  extensions = '-x tables' + ' ' + '-x "toc(title=In This Document)"'
  command = 'markdown' + ' ' + extensions + ' ' + raw_file
  p = subprocess.Popen(command, stdout = subprocess.PIPE, shell = True)
  return p.communicate()[0]


# read just the title (first heading) from a source page
def get_title(raw_file):
  for line in open(raw_file, 'r'):
    if '#' in line:
      return line.strip(' #\n')
  return ""


# directory to compile the site to (will be clobbered during build!)
HTML_DIR = 'out'
# directory to look in for markdown source files
SRC_DIR = 'src'
# directory to look in for html templates
TEMPLATE_DIR = 'templates'

# filenames of templates to load, in order
TEMPLATE_LIST = ['includes', 'header', 'sidebar', 'main', 'footer'] 

t = ""
for f in TEMPLATE_LIST:
  t += open(os.path.join(TEMPLATE_DIR, f), 'r').read()
template = string.Template(t)

if os.path.exists(HTML_DIR):
  shutil.rmtree(HTML_DIR)

os.mkdir(HTML_DIR)

category = 'home'
parents = {}
for curdir, subdirs, files in os.walk(SRC_DIR):
  print 'Processing %s...'  % (curdir,),
  outdir = [x for x in curdir.split(os.path.sep) if x]
  outdir[0] = HTML_DIR
  if len(outdir) == 2:
    category = outdir[-1]
  outdir = os.path.join(*outdir)

  for subdir in subdirs:
    os.mkdir(os.path.join(outdir, subdir))

  parentdir = os.path.dirname(curdir)
  if parentdir in parents:
    parent = parents[parentdir]
  else:
    parent = ('', '')

  if 'sidebar.md' in files:
    sidebar = markdown(os.path.join(curdir, 'sidebar.md'))
    del files[files.index('sidebar.md')]
  else:
    sidebar = parent[0]

  if 'sidebar2.md' in files:
    sidebar2 = markdown(os.path.join(curdir, 'sidebar2.md'))
    del files[files.index('sidebar2.md')]
  else:
    sidebar2 = parent[1]

  parents[curdir] = (sidebar, sidebar2)

  for f in files:
    print ' .',
    if f.endswith('.md'):
      main = markdown(os.path.join(curdir, f))
      final = template.safe_substitute(main=main, sidebar=sidebar, sidebar2=sidebar2, \
          category=category, title=get_title(os.path.join(curdir, f)))
    
      html = file(os.path.join(outdir, f.replace('.md', '.html')), 'w')
      html.write(final)
    else:
      shutil.copy(os.path.join(curdir, f), os.path.join(outdir, f))
  print

print 'Done.'
