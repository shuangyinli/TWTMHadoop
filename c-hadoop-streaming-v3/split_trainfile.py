#!/usr/bin/python 

import sys
import random
from collections import defaultdict as ddict

def load_docid_mapperid(doc_mappid_file):
	docid_mapperid = dict()
	for line in file(doc_mappid_file):
		cols = line.strip().split()
		docid = int(cols[0])
		mapperid = int(cols[1])
		docid_mapperid[docid] = mapperid
	return docid_mapperid



def run(trainfile, doc_mappid_file, out, num_mapper):
	docid_mapperid = load_docid_mapperid(doc_mappid_file)
	mapperid_cnt = ddict(int)
	for mapperid in docid_mapperid.values():
		mapperid_cnt[mapperid] += 1

	mapperid_partid = dict()
	partid_cnt = ddict(int)
	for mappid in sorted(mapperid_cnt.keys(), key=lambda x:-mapperid_cnt[x]):
		min_partid = -1
		for partid in range(num_mapper):
			if min_partid == -1 or partid_cnt[partid] < partid_cnt[min_partid]:
				min_partid = partid
		mapperid_partid[mappid] = min_partid
		partid_cnt[min_partid] += mapperid_cnt[mappid]

	fps = dict()
	
	for partid in mapperid_partid.values():
		fps[partid] = open('%s/part-%d' % (out, partid),'w')
	
	num_line = 0
	for line in file(trainfile):
		mappid = docid_mapperid[num_line]
		print >> fps[mapperid_partid[mappid]], '%d %s' % (num_line, line[:-1])
		num_line += 1

	for fp in fps.values():
		fp.close()


if __name__=='__main__':
	if len(sys.argv) != 5:
		print >> sys.stderr, 'usage: <train file> <doc mappid file> <out dir> <num_mapper>'
		sys.exit(-1)
	run(sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]))


