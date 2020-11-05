#include "CLbplusTree.h"
#include <map>
#include <algorithm>
std::map<SELECT_TYPE, int> selectDispatcher;

void CLbplusTree::adjustRoot()
{
	CLbplusTreeNode* newRoot = m_Root->getParentNode();
	while (newRoot != NULL)
	{
		m_Root = newRoot;
		newRoot = m_Root->getParentNode();
	}
}

bool CLbplusTree::check(CLbplusTreeNode* node, KeyType key)
{
	if (!node)
	{
		return false;
	}
	int keyIndex = node->getKeyIndex(key);
	int childIndex = node->getChildIndex(key, keyIndex); // 孩子结点指针索引
	if (keyIndex < node->getKeyNum() && key == node->getKeyValue(keyIndex))
	{
		return true;
	}
	if (node->getIsLeaf()) {
		return false;
	}
	else {
		return check(((CLbplusTreeInnerNode*)node)->getChild(childIndex), key);
	}
}

bool CLbplusTree::insert(KeyType key, const DataType& data) {

	SearchResult searchResult;
	search(m_Root, key, searchResult);

	CLbplusTreeLeafNode* childNode = searchResult.targetNode;
	if (searchResult.targetNode != NULL)
	{
		childNode->insert(key, data);

		if (key > m_MaxKey)  // 更新最大键值
		{
			m_MaxKey = key;
		}		
	}
	else
	{
		m_Root = new CLbplusTreeLeafNode();
		m_DataHead = (CLbplusTreeLeafNode*)m_Root;
		m_MaxKey = key;
		((CLbplusTreeLeafNode*)m_Root)->insert(key, data);

	}
	adjustRoot();
	//if (m_Root != NULL)
	//{
	//	if (m_Root->getKeyNum() >= UPPER_BOUND_KEYNUM) {
	//		CLbplusTreeInnerNode* innerNode = new CLbplusTreeInnerNode();
	//		innerNode->setChild(0, m_Root);
	//		m_Root->split(innerNode, 0);   
	//		m_Root = innerNode;
	//	}
	//	if (key > m_MaxKey)  // 更新最大键值
	//	{
	//		m_MaxKey = key;
	//	}
	//}
	//else
	//{
	//	m_Root = new CLbplusTreeLeafNode();
	//	m_DataHead = (CLbplusTreeLeafNode*)m_Root;
	//	m_MaxKey = key;
	//}
	//
	//insertToNode(m_Root, key, data);
	return true;
}

bool CLbplusTree::insertToNode(CLbplusTreeNode* node, KeyType key, const DataType& data)
{
	if (node == NULL)
	{
		return false;
	}
	if (node->getIsLeaf()) 
	{
		((CLbplusTreeLeafNode*)node)->insert(key, data);
	}
	else 
	{
		CLbplusTreeInnerNode* innerNode = ((CLbplusTreeInnerNode*)node);
		int keyIndex = innerNode->getKeyIndex(key);
		int childIndex = innerNode->getChildIndex(key, keyIndex); // 孩子结点指针索引
		CLbplusTreeNode* childNode = innerNode->getChild(childIndex);
		if (childNode->getKeyNum() >= UPPER_BOUND_KEYNUM)
		{
			childNode->split(node, childIndex);
			if (node->getKeyValue(keyIndex) <= key)   // 分割后重新确定目标子结点
			{
				childNode = innerNode->getChild(childIndex + 1);
			}
		}
		insertToNode(childNode, key, data);
	}
	return true;
}

bool CLbplusTree::select(KeyType lowerBound, KeyType upperBound, bool lowerContain, bool upperContain, std::vector<DataType> & selectResult)
{
	if (lowerBound > upperBound)
	{
		return false;
	}
	SearchResult beginR, finalR;
	search(m_Root,lowerBound, beginR);
	search(m_Root,upperBound, finalR);
	CLbplusTreeLeafNode *beginNode = beginR.targetNode, *finalNode = finalR.targetNode;
	int beginIndex = beginR.keyIndex, finalIndex = finalR.keyIndex;
	if (beginR.targetNode->getKeyValue(beginR.keyIndex) < lowerBound || (beginR.targetNode->getKeyValue(beginR.keyIndex) == lowerBound && !lowerContain)) {
		beginIndex += 1;
	}
	if (finalR.targetNode->getKeyValue(finalR.keyIndex) > upperBound || (finalR.targetNode->getKeyValue(finalR.keyIndex) == upperBound && !upperContain)) {
		finalIndex -= 1;
	}
	if (beginNode == finalNode)
	{
		for (int i = beginIndex; i <= finalIndex; ++i)
		{
			selectResult.push_back(beginNode->getData(i));
		}
	}
	else 
	{
		for (CLbplusTreeLeafNode* pLeaf = beginNode; pLeaf != finalNode; pLeaf = pLeaf->getRightSibling())
		{
			for (int i = beginIndex; i < pLeaf->getKeyNum(); ++i)
			{
				selectResult.push_back(pLeaf->getData(i));
			}
			beginIndex = 0;
		}
		for (int i = 0; i <= finalIndex; ++i)
		{
			selectResult.push_back(finalNode->getData(i));
		}
	}
	std::sort<std::vector<DataType>::iterator>(selectResult.begin(), selectResult.end());
	return true;
}

bool CLbplusTree::select(KeyType key, SELECT_TYPE selectType, std::vector<DataType>& selectResult)
{
	if (m_Root == NULL)
	{
		return false;
	}

	switch (selectType)
	{
	case SELECT_TYPE::EQ:
		if (key > m_MaxKey || key < m_DataHead->getKeyValue(0))
		{
			return false;
		}
		else 
		{
			SearchResult searchResult;
			search(m_Root, key, searchResult);
			if (searchResult.targetNode->getKeyValue(searchResult.keyIndex) == key) {
				selectResult.push_back(searchResult.targetNode->getData(searchResult.keyIndex));
			}
		}
		break;
	case SELECT_TYPE::GT:
	case SELECT_TYPE::GE:
	{
		CLbplusTreeLeafNode* beginNode = m_DataHead;
		int beginIndex = 0;
		if (key >= m_DataHead->getKeyValue(0))
		{
			SearchResult searchResult;
			search(m_Root, key, searchResult);
			beginNode = searchResult.targetNode;
			beginIndex = searchResult.keyIndex;
			if (beginNode->getKeyValue(beginIndex) < key || (beginNode->getKeyValue(beginIndex) == key && selectType == SELECT_TYPE::GT))
			{
				beginIndex += 1;
			}
		}
		//顺序添加查找结果
		for (CLbplusTreeLeafNode* pLeaf = beginNode; pLeaf != NULL; pLeaf = pLeaf->getRightSibling())
		{
			for (int i = beginIndex; i < pLeaf->getKeyNum(); ++i)
			{
				selectResult.push_back(pLeaf->getData(i));
			}
			//第二个节点开始，每个key都要添加
			beginIndex = 0;
		}
	}
		break;
	case SELECT_TYPE::LT:
	case SELECT_TYPE::LE:
	{
		CLbplusTreeLeafNode* finalNode = NULL;
		int finalIndex = -1;
		if (key <= m_MaxKey)
		{
			SearchResult searchResult;
			search(m_Root, key, searchResult);
			finalNode = searchResult.targetNode;
			finalIndex = searchResult.keyIndex;
			if (finalNode->getKeyValue(finalIndex) > key || (finalNode->getKeyValue(finalIndex) == key && selectType == SELECT_TYPE::LT))
			{
				finalIndex -= 1;
			}
		}
		for (CLbplusTreeLeafNode* pLeaf = m_DataHead; pLeaf != finalNode; pLeaf = pLeaf->getRightSibling())
		{
			for (int i = 0; i < pLeaf->getKeyNum(); ++i)
			{
				selectResult.push_back(pLeaf->getData(i));
			}
		}
		if (finalNode != NULL)
		{
			for (int i = 0; i <= finalIndex; i++)
			{
				selectResult.push_back(finalNode->getData(i));
			}
		}
	}
		break;
	default:
		break;
	}
	std::sort<std::vector<DataType>::iterator>(selectResult.begin(), selectResult.end());

	return true;
}

void CLbplusTree::search(CLbplusTreeNode* pNode, KeyType key, SearchResult& result)
{
	if (pNode == NULL)
	{
		return;
	}
	int keyIndex = pNode->getKeyIndex(key);
	int childIndex = pNode->getChildIndex(key, keyIndex); // 孩子结点指针索引
	if (pNode->getIsLeaf())
	{
		result.keyIndex = keyIndex;
		result.targetNode = (CLbplusTreeLeafNode*)pNode;
		return;
	}
	else
	{
		return search(((CLbplusTreeInnerNode*)pNode)->getChild(childIndex), key, result);
	}
}