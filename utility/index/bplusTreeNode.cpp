#include "bplusTreeNode.h"
int CLbplusTreeNode::getKeyIndex(KeyType key)const
{
	int lower = 0, upper = m_KeyNum - 1;

	while (lower < upper) {
		int cmpIndex = (lower + upper) / 2;
		if (key > m_KeyValues[cmpIndex])
		{
			lower = cmpIndex + 1;
		}
		else 
		{
			upper = cmpIndex ;
		}
	}
	while (lower < m_KeyNum - 1 && lower < UPPER_BOUND_KEYNUM && (m_KeyValues[lower] == m_KeyValues[lower + 1]))
	{
		lower = lower + 1;
	}
	return lower;
}

int CLbplusTreeInnerNode::getChildIndex(KeyType key, int keyIndex)const
{
	if (key >= getKeyValue(keyIndex))
	{
		return keyIndex + 1;
	}
	else
	{
		return keyIndex;
	}

}


void CLbplusTreeLeafNode::insert(KeyType key, const DataType& data)
{
	int i;
	for (i = m_KeyNum; i >= 1 && m_KeyValues[i - 1] > key; --i)
	{
		setKeyValue(i, m_KeyValues[i - 1]);
		setData(i, m_Datas[i - 1]);
	}
	setKeyValue(i, key);
	setData(i, data);
	setKeyNum(m_KeyNum + 1);

	if (getKeyNum() > UPPER_BOUND_KEYNUM)
	{
		CLbplusTreeNode* parentNode = getParentNode();
		if (parentNode != NULL)
		{
			int keyIndex = parentNode->getKeyIndex(key);
			int childIndex = parentNode->getChildIndex(key, keyIndex);
			split(parentNode, childIndex);

		}
		else {
			CLbplusTreeInnerNode* innerNode = new CLbplusTreeInnerNode();
			innerNode->setChild(0, this);
			split(innerNode, 0);
			setParentNode(innerNode);
		}
	}


}

void CLbplusTreeInnerNode::insert(int newKeyIndex, int newChildIndex, KeyType newKey, CLbplusTreeNode* newChildNode)
{
	int i;
	for (i = getKeyNum(); i > newKeyIndex; --i)//将父节点中的childIndex后的所有关键字的值和子树指针向后移一位
	{
		setChild(i + 1, m_Childs[i]);
		setKeyValue(i, m_KeyValues[i - 1]);
	}
	if (i == newChildIndex)
	{
		setChild(i + 1, m_Childs[i]);
	}
	setChild(newChildIndex, newChildNode);
	setKeyValue(newKeyIndex, newKey);
	setKeyNum(m_KeyNum + 1);

	if (getKeyNum() > UPPER_BOUND_KEYNUM)
	{
		CLbplusTreeNode* parentNode = getParentNode();
		if (parentNode != NULL)
		{
			int keyIndex = parentNode->getKeyIndex(newKey);
			int childIndex = parentNode->getChildIndex(newKey, keyIndex);
			split(parentNode, childIndex);

		}
		else {
			CLbplusTreeInnerNode* innerNode = new CLbplusTreeInnerNode();
			innerNode->setChild(0, this);
			split(innerNode, 0);
			setParentNode(innerNode);

		}
	}
}

void CLbplusTreeLeafNode::split(CLbplusTreeNode* parentNode, int childIndex)
{
	CLbplusTreeLeafNode* newNode = new CLbplusTreeLeafNode();//分裂后的右节点
	newNode->setParentNode(parentNode);
	setKeyNum(LOWER_BOUND_KEYNUM + 1);
	newNode->setKeyNum(LOWER_BOUND_KEYNUM + 1);
	CLbplusTreeLeafNode* rightNode = getRightSibling();
	newNode->setRightSibling(rightNode);
	setRightSibling(newNode);
	newNode->setLeftSibling(this);
	if (rightNode != NULL)
	{
		rightNode->setLeftSibling(newNode);
	}

	for (int i = 0; i < LOWER_BOUND_KEYNUM + 1; ++i)// 拷贝关键字的值
	{
		newNode->setKeyValue(i, m_KeyValues[i + LOWER_BOUND_KEYNUM + 1]);
	}
	for (int i = 0; i < LOWER_BOUND_KEYNUM + 1; ++i)// 拷贝数据
	{
		newNode->setData(i, m_Datas[i + LOWER_BOUND_KEYNUM + 1]);
	}
	((CLbplusTreeInnerNode*)parentNode)->insert(childIndex, childIndex + 1, m_KeyValues[LOWER_BOUND_KEYNUM + 1], newNode);
}

void CLbplusTreeInnerNode::split(CLbplusTreeNode* parentNode, int childIndex) 
{
	CLbplusTreeInnerNode* newNode = new CLbplusTreeInnerNode();//分裂后的右节点
	newNode->setParentNode(parentNode);
	newNode->setKeyNum(LOWER_BOUND_KEYNUM + 1);
	int i;
	for (i = 0; i < LOWER_BOUND_KEYNUM + 1; ++i)// 拷贝关键字的值
	{
		newNode->setKeyValue(i, m_KeyValues[i + LOWER_BOUND_KEYNUM+1]);
	}
	for (i = 0; i < LOWER_BOUND_CHILDNUM+1 ; ++i) // 拷贝孩子节点指针
	{
		newNode->setChild(i, m_Childs[i + LOWER_BOUND_CHILDNUM]);
	}
	setKeyNum(LOWER_BOUND_KEYNUM);  //更新左子树的关键字个数
	((CLbplusTreeInnerNode*)parentNode)->insert(childIndex, childIndex + 1, m_KeyValues[LOWER_BOUND_KEYNUM], newNode);
}

