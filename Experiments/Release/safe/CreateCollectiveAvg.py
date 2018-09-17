#!/usr/bin/env python
import os



if __name__ == '__main__':

  map = {}
  mapCounters = {}
 
  for root, dirs, files in os.walk('./'):
    for file in files:
      if file != 'RemoveExtraLines.py' and file != 'AveragesCreate.py' and file != "CreateCollectiveAvg.py" and file != 'CollectiveAverages.txt':
        with open(file, 'r') as fin:
          data = fin.read().splitlines(True)
          for line in data:
            strings = line.split(' ')
            if strings[1] not in map.keys():
              map[ strings[1] ] = 0
              mapCounters[ strings[1] ] = 0
            map[ strings[1] ] += float( strings[0] )
            mapCounters[ strings[1] ] += 1
    
  
  with open("CollectiveAverages.txt", 'w') as fout:
    for experiment in map.keys():
      if mapCounters[experiment] != 0:
        map[experiment] /= mapCounters[experiment]
    
      fout.write( str(map[experiment]) + " :" + experiment )