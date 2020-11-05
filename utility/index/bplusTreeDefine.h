#pragma once
#include <iostream>
//每个结点所包含的关键字个数有上界和下界。用一个被称为B树的最小度数(minmum degree）的规定整数 表示这里的界。
#define MIN_DEGREE 2
//除了根结点以外的每个结点必须至少有 t -1个关键字
#define LOWER_BOUND_KEYNUM (MIN_DEGREE - 1)
//每个结点至多可包含2t - 1个关键字。
#define UPPER_BOUND_KEYNUM ((MIN_DEGREE * 2) - 1)
 //除了根结点以外的每个结点必须至少有 t -1个关键字
#define LOWER_BOUND_CHILDNUM (LOWER_BOUND_KEYNUM+1)
//每个结点至多可包含2t - 1个关键字。
#define UPPER_BOUND_CHILDNUM (UPPER_BOUND_KEYNUM+1)
enum NODE_TYPE { INTERNAL, LEAF };        // 结点类型：内结点、叶子结点
enum SIBLING_DIRECTION { LEFT, RIGHT };   // 兄弟结点方向：左兄弟结点、右兄弟结点
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