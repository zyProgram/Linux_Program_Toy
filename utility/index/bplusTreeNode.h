#pragma once
#include <iostream>
#include "bplusTreeDefine.h"

class CLbplusTreeNode {
public:
	CLbplusTreeNode() { setKeyNum(0); setParentNode(NULL); }
	virtual ~CLbplusTreeNode() { setKeyNum(0); }

	bool getIsLeaf() const { return m_IsLeaf; }
	void setIsLeaf(bool isLeaf) { m_IsLeaf = isLeaf; }
	int getKeyNum() const { return m_KeyNum; }
	void setKeyNum(int n) { m_KeyNum = n; }
	KeyType getKeyValue(int i) const { return m_KeyValues[i]; }
	void setKeyValue(int i, KeyType key) { m_KeyValues[i] = key; }
	void setParentNode(CLbplusTreeNode* parent) { m_ParentNode = parent; }
	CLbplusTreeNode* getParentNode() const { return m_ParentNode; }
	int getKeyIndex(KeyType key)const;  // 找到键值在结点中存储的下标
	// 纯虚函数，定义接口
	virtual void split(CLbplusTreeNode* parentNode, int childIndex) = 0; // 分裂结点
	virtual void clear() = 0; // 清空结点，同时会清空结点所包含的子树结点
	virtual int getChildIndex(KeyType key, int keyIndex)const = 0;  // 根据键值获取孩子结点指针下标
protected:
	CLbplusTreeNode* m_ParentNode;
	bool m_IsLeaf;
	int m_KeyNum;
	KeyType m_KeyValues[UPPER_BOUND_KEYNUM + 1];
};

class CLbplusTreeLeafNode : public CLbplusTreeNode
{
public:
	CLbplusTreeLeafNode(): CLbplusTreeNode() {
		setIsLeaf(true);
		setLeftSibling(NULL);
		setRightSibling(NULL);
	};
	virtual ~CLbplusTreeLeafNode() {  };

	void insert(KeyType key, const DataType& data);
	DataType getData(int i) { return m_Datas[i]; }
	void setData(int i, DataType data) { m_Datas[i] = data; }
	CLbplusTreeLeafNode* getRightSibling() { return m_RightSibling; }
	void setRightSibling(CLbplusTreeLeafNode* right) { m_RightSibling = right; }
	CLbplusTreeLeafNode* getLeftSibling() { return m_LeftSibling; }
	void setLeftSibling(CLbplusTreeLeafNode* left) { m_LeftSibling = left; }
	// 虚函数，继承
	virtual void split(CLbplusTreeNode* parentNode, int childIndex); // 分裂结点
	virtual void clear() {}; // 清空结点，同时会清空结点所包含的子树结点
	virtual int getChildIndex(KeyType key, int keyIndex)const { return keyIndex;  };  // 根据键值获取孩子结点指针下标
private:
	CLbplusTreeLeafNode* m_LeftSibling;
	CLbplusTreeLeafNode* m_RightSibling;
	DataType m_Datas[UPPER_BOUND_KEYNUM + 1];
};

class CLbplusTreeInnerNode : public CLbplusTreeNode
{
public:
	CLbplusTreeInnerNode(): CLbplusTreeNode() {
		setIsLeaf(false);
		
	}
	virtual ~CLbplusTreeInnerNode() {}
	CLbplusTreeNode* getChild(int i) const { return m_Childs[i]; }
	void setChild(int i, CLbplusTreeNode* child) { m_Childs[i] = child; }
	void insert(int keyIndex, int childIndex, KeyType key, CLbplusTreeNode* childNode);
	// 虚函数，继承
	virtual void split(CLbplusTreeNode* parentNode, int childIndex); // 分裂结点
	virtual void clear() {}; // 清空结点，同时会清空结点所包含的子树结点
	virtual int getChildIndex(KeyType key, int keyIndex)const ;  // 根据键值获取孩子结点指针下标
private:
	CLbplusTreeNode* m_Childs[UPPER_BOUND_CHILDNUM + 1];
};