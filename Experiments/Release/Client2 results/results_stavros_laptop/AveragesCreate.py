#!/usr/bin/env python
import os



if __name__ == '__main__':

  # ziph is zipfile handle
  with open("Averages.txt", 'w') as fout:
    for root, dirs, files in os.walk('./'):
      for file in files:
        if file != 'RemoveExtraLines.py' and file != 'AveragesCreate.py' and file != 'Averages.txt':
          with open(file, 'r') as fin:
            data = fin.read().splitlines(True)
            average = 0;
            counter = 0;
            for numberString in data:
              try:
                average += float( numberString )
                counter += 1
              except ValueError:
                print("wrong value - ignore: " + str(numberString) )
            if counter > 0:
              average /= counter;
            fout.write( str(average) + " :" + file + '\n' )