#!/usr/bin/env python
import os



if __name__ == '__main__':

  # ziph is zipfile handle
  for root, dirs, files in os.walk('./'):
    for file in files:
      if file != 'RemoveExtraLines.py':
        with open(file, 'r') as fin:
          data = fin.read().splitlines(True)
        with open(file, 'w') as fout:
          fout.writelines(data[3:(len(data) -2) ])