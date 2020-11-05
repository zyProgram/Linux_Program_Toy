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
	//��Χ���ң�����Ϊ���½��keyֵ��ָʾ�Ƿ�������½��bool������
	bool select(KeyType lowerBound, KeyType upperBound, bool lowerContain, bool upperContain, std::vector<DataType>& selectResult);
	bool select(KeyType key, SELECT_TYPE selectType, std::vector<DataType>& selectResult);

private:
	//����nodeΪ���ڵ��������Ѱ��key
	void adjustRoot();
	bool check(CLbplusTreeNode* node ,KeyType key);
	bool insertToNode(CLbplusTreeNode* node, KeyType key, const DataType& data);
	void search(CLbplusTreeNode* pNode, KeyType key, SearchResult& result);

private:
	CLbplusTreeNode* m_Root;
	CLbplusTreeLeafNode* m_DataHead;
	KeyType m_MaxKey;  // B+���е�����
};

