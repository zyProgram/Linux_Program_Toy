#pragma once
#include <iostream>
#include "CLbplusTree.h"
#include "bplusTreeDefine.h"
#include "bplusTreeNode.h"
#include <vector>
class CLDataIndex
{
public:
	CLDataIndex() {
	};
	~CLDataIndex() {};
	/**
	 *
	 * @param datePtr 所有数据某一列的元素向量
	 * @param row 所有数据行号
	 * @param count
	 */
	void createIndex(std::vector<int64_t> datePtr, std::vector<int> row);
private:
	CLbplusTree m_BplusTree;
};

