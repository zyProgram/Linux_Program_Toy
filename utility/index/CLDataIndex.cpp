#include "CLDataIndex.h"

#include <time.h>
#include <algorithm>
#include "assert.h"
void CLDataIndex::createIndex(std::vector<int64_t> datas, std::vector<int> rowIndexs)
{
    assert(datas.size() == rowIndexs.size());
    int count = datas.size();
	if (datas.size() == 0 || rowIndexs.size() != datas.size())
	{
		return;
	}
	for (int i = 0; i < count; i++)
	{
		m_BplusTree.insert(datas[i], rowIndexs[i]); 
	}
}
