#pragma once
#include <iostream>
//ÿ������������Ĺؼ��ָ������Ͻ���½硣��һ������ΪB������С����(minmum degree���Ĺ涨���� ��ʾ����Ľ硣
#define MIN_DEGREE 2
//���˸���������ÿ�������������� t -1���ؼ���
#define LOWER_BOUND_KEYNUM (MIN_DEGREE - 1)
//ÿ���������ɰ���2t - 1���ؼ��֡�
#define UPPER_BOUND_KEYNUM ((MIN_DEGREE * 2) - 1)
 //���˸���������ÿ�������������� t -1���ؼ���
#define LOWER_BOUND_CHILDNUM (LOWER_BOUND_KEYNUM+1)
//ÿ���������ɰ���2t - 1���ؼ��֡�
#define UPPER_BOUND_CHILDNUM (UPPER_BOUND_KEYNUM+1)
enum NODE_TYPE { INTERNAL, LEAF };        // ������ͣ��ڽ�㡢Ҷ�ӽ��
enum SIBLING_DIRECTION { LEFT, RIGHT };   // �ֵܽ�㷽�����ֵܽ�㡢���ֵܽ��
enum class SELECT_TYPE {
    EQ = 0,
    GT,
    LT,
    GE,
    LE
}; //EQUAL, GREATER THAN ,LESS THAN, GREATER THAN OR EQUAL, LESS THAN OR EQUAL
#define KeyType int64_t
#define DataType int64_t
#define KeyTypeMax INT64_MAX
#define KeyTypeMin INT64_MIN