#pragma once
#include "bplusTreeDefine.h"
#include "bplusTreeNode.h"
#include <vector>
struct SearchResult
{
	int keyIndex;
	CLbplusTreeLeafNode* targetNode;
	SearchResult() { keyIndex = -1; targetNode = NULL; }
};
class CLbplusTree
{
public:
	CLbplusTree() {
		m_Root = NULL;
		m_DataHead = NULL;
	};
	~CLbplusTree() {};
	bool insert(KeyType key, const DataType &data);
	//范围查找，输入为上下界的key值和指示是否包含上下界的bool变量。
	bool select(KeyType lowerBound, KeyType upperBound, bool lowerContain, bool upperContain, std::vector<DataType>& selectResult);
	bool select(KeyType key, SELECT_TYPE selectType, std::vector<DataType>& selectResult);

private:
	//在以node为根节点的子树中寻找key
	void adjustRoot();
	bool check(CLbplusTreeNode* node ,KeyType key);
	bool insertToNode(CLbplusTreeNode* node, KeyType key, const DataType& data);
	void search(CLbplusTreeNode* pNode, KeyType key, SearchResult& result);

private:
	CLbplusTreeNode* m_Root;
	CLbplusTreeLeafNode* m_DataHead;
	KeyType m_MaxKey;  // B+树中的最大键
};

