---
typora-root-url: ./
---

# Linux_Program_Toy

# Linux程序设计实现

## 1 需求分析

### 1.1 课程基本功能需求

本次Linux高级程序设计的基本要求是需要实现一个数据表管理系统，该系统能够存储一张数据表，数据表中每个数据的属性数量都有100个，所支持的存储数据条目数量不少于100万条，数据表最终需要存储在磁盘上。

按照课程功能，拟定需要从接口，存储，索引，并发四个方面进行总体需求分析。

### 1.2 接口功能需求

按照课程基本要求，整个系统管理了一张数据表，该系统需要提供基础的创建数据表(Create),数据插入(Append),数据查询Read(查询)功能，以接口形式暴露。除此之外，由于数据量过于庞大，需要建立索引机制，那么系统还需要提供从特定属性建立索引的功能，该功能需要通过一个建立索引接口暴露给上层。

### 1.3 存储功能需求

整个系统应能够存储不少于100w行的文件数据。大量的数据不可能一直处在内存中，文件数据需要能够dump到磁盘进行存储。除此之外，索引文件在程序结束时也需要能够存储在内存中，

### 1.4 索引需求

按照上述要求描述，必然需要提供索引支持数据的查询操作，索引能够根据用户数据中100个属性中的某一个属性构建，可参考采用b树或者b+树实现。

### 1.5 并发需求

在磁盘进行io时，io处理延时较高，内部采用多线程模型进行数据的读取写入。



 

## 2 总体设计

### 2.1 按照需求，总体设计

按照功能需求，整个程序以对多个文件的读写操作为基础，实现对大型数据的操作。因此以文件操作为基础，对外暴露文件表操作的形式，形成了完整的数据管理系统(Data Manage System,后文简称DMS)。 

针对各个功能点，设计如下

1、存储的解决：

​	封装系统API接口，实现基本文件的操作，然后封装成行写入的形式，方便用户以结构化的数据写入。同时，系统关机后，相关的存储数据需要被回复，所以需要记录元数据和用户第一次的配置数据。

2、并发。

​	底层提供线程池，线程池存在任务队列，所有工作线程去抢夺线程池中的任务，工作线程数量应该为当前每个CPU核数 x CPU数量。

3、索引

​	整个数据的索引以b+树实现，B+树按照数据和存储的行号形成键值对进行存储。用户可以指定任意一列数据属性作为键值，每一次append操作会为数据添加一个行号，行号作为索引的value。然后通过调用底层存储的表数据进行查询。

4、统一接口

​	封装一个Manager调度底层资源，同时向上层提供整个系统的append和read功能，实现时，既对外既可以暴露出阻塞的接口又提供非阻塞的多线程接口。

### 2.2 总体设计框图

按照设计，我们自上往下将DMS设计如下图所示

 ![model](pic\model.bmp)



### 2.3 各模块简介

 1、文件管理模块

负责处理文件的打开，创建，关闭，数据写入，数据读取，刷新到磁盘等基本操作

负责对应用所需要存储的数据进行封装，由于各种类型的文件对于底层文件操作的需求不一样，使用如下

| 模块      | 文件操作需求                                                 |
| :-------- | ------------------------------------------------------------ |
| Logger    | 写入数据内容需要添加时间戳前缀，和回车后缀                   |
| metadata  | 能够进行readall操作，从磁盘中读取元数据所有字节到buffer中    |
| configure | 配置文件的需求同metadata                                     |
| tableFile | 写入数据需要添加4字节行号作为前缀，回车作为后缀，所有数据按照行存储。 |

2、文件操作统一管理模块

负责用于对内进行各个逻辑块的调用，协调内部各个类的运作，如数据重启的恢复，多线程任务的提交。

负责对外的统一接口，用户可以直接通过本模块进行的原始数据的操作，涉及创建，插入，读取，建立索引等。

3、接口层

本层主要面向用户，提供了方便的用户数据构建，如创建一个double类型，char类型，int类型的数据，构建出的数据可以更方便地变成用户的行号，本程序主要使用100个int形成的vector作为用户的基本属性列表。通过该层可以方便地将数据转换为字节流，更加简便地存入文件中。

## 3 详细设计与实现

本章节主要介绍代码的主要功能实现逻辑，首先介绍了本次代码的书写规范，然后采用了代码实现的时间先后，采用自下而上的封装方式叙述整个代码各个模块的详细实现。整个代码的类图实现如下图所示

![alluml](pic\alluml.bmp)

### 3.1代码规范

​	在介绍代码设计之前，由于代码不是一次性书写完成，前后周期跨度约为一个月，为了让代码更加整洁，开发方便。首先需要约定好代码规范。在代码实现时，使用本学期李林老师课上常用代码规范，结合自身比较熟练的代码标准，现将本程序所考虑到的代码规范规约如下

**1、类的命名采用CL开头，后接大驼峰标识的名字。如文件管理模块类'CLFileManger'  、文件表处理类 'CLTableFile '、基础文件类'CLFile'等**

**2、结构体的命名采用SL开头，后接大驼峰标识的名字。如用户进行多线程写数据回调参数结构体‘SLReaderMethodParam’**

**3、类内成员变量采用‘_’开头并且以‘\_'分割的小写字母命名，如'\_pool' 线程池变量、‘\_meta_file’元数据文件变量**

**4、类内私有方法采用‘_'开头的大写字母，如扩展本地文件函数'\_ExtendLocalCLFile'，底层写文件操作‘\_NormalWrite'**

**5、类内公有方法使用大驼峰命名，如添加一行数据函数‘Append’**

**6、类内静态成员变量采用’\_s\_‘开头的小写字母命名，如单例中的静态变量’\_s\_instance'****

**7、常规的局部变量，inline函数等，均采用小驼峰命名，如文件名'filename',文件类指针'pCLFile '***

**8、命名空间采用小写字母命名，如个人代码标识”zy“，”dms“，”thread“**

**9、注释只写关键代码的注释，放置于关键代码的右方，如果右侧间距不够，或者不够美观，可在代码上方进行注释****

### 3.2 底层文件模块实现

文件写缓存，由于文件一次io需要消耗大量时间，所以采用了先写缓存的形式，缓存大小按照课上所学，效率最优为4096字节。当超过4096字节后，再进行写入。整个模块对外提供接口如下

```c++
bool Write(const char *buffer,int total);
bool Read(long offset, char *buffer, int total);
bool ReadAll(char *buffer, int total,ssize_t &realCount);
void Flush();
void Close();
void Clear();
```

#### 3.2.1 文件模块类

![image-20201115235023428](/pic/clfile.png)

#### 3.2.2 核心代码

CLFile暴露给上层的写入接口，不满一个buffer的数据先写入到了buffer中，超出了后再调用_NormalWrite，进行系统调用write的写入

```c++
bool zy::file::CLFile::Write(const char *buffer, int total) {
    int remain = _GetReamin();
    if(total < remain){
        memcpy(_buffer+_used,buffer,total);
        _used += total;
    }else{
        int offset = remain;
        //先dump 当前buffer到磁盘
        memcpy(_buffer+_used,buffer,remain);
        _NormalWrite(_buffer,MAX_FILE_BUF_SIZE);
        int left = total - remain;
        while (left > MAX_FILE_BUF_SIZE){
            memcpy(_buffer,buffer+offset,MAX_FILE_BUF_SIZE);
            _used = MAX_FILE_BUF_SIZE;
            _NormalWrite(_buffer,_used);
            left -= MAX_FILE_BUF_SIZE;
            offset += MAX_FILE_BUF_SIZE;
            memset(_buffer,0,MAX_FILE_BUF_SIZE);
            _used = 0;
        }
        memcpy(_buffer+_used,buffer+offset, left);
        _used += left;
    }
    return true;
}
```

CLFile暴露给上层的读数据接口，读数据此处的处理比较粗糙，由于用户的读写顺序是不可预知的，所以采用的方法是，每一次读取会进行一次read系统调用

```c++
bool zy::file::CLFile::Read(long offset, char *buffer, int total) {
    lseek(_open_file_handle,offset,SEEK_SET);
    if (read(_open_file_handle,buffer,total) < 0){
        std::cout<<"error read:"<<strerror(errno)<<std::endl;
        return false;
    }
    return true;
}
```

#### 3.2.3 表文件数据读写模块

有了底层的文件封装，那么我们可以简单地进行基础数据的读写，而需求是按照数据进行一行一行写入或者一行一行进行读取，整个数据以行作为单位进行读写

```c++
void Flush();
/**
* 写入一行数据，加上前缀row 和 末尾\n
* @param msg
* @return true or false
*/
bool WriteRow(const std::string &msg);
bool ReadRow(int rowLine, char *buffer, int size);
```

#### 3.2.4表文件数据读写流程图

#### 3.2.5 核心代码

读取一行数据，根据行号计算出偏移量，读取一整行数据

```c++
bool ReadRow(int rowLine, char *buffer, int size) override{
    if(size < _row_size - _s_header_size - _s_suffix_size){
        std::cout<<"invalid read parameter : "<<size<< " < "<<_row_size - _s_header_size - _s_suffix_size <<std::endl;
        std::abort();
        return false;
    }
    int offset = _row_size * (((rowLine-1) > 0) ? rowLine-1:0);
    char lowBuf[size + CLTableFile::_s_header_size+CLTableFile::_s_suffix_size];
    bool ret = _table_file->Read(offset,lowBuf , _row_size - CLTableFile::_s_header_size);

    if(ret){
        memcpy(buffer, lowBuf + CLTableFile::_s_header_size,size);
    }
    return ret;
}
```

写入一行数据,写入数据包括写入一个4字节的行号，加上具体数据，然后帮助用户加上一个回车后缀

```c++
bool WriteRow(const char *buffer, int size) override{
    int id = _cur_row+1;
    char headbuf[CLTableFile::_s_header_size];
    memcpy(headbuf,&id,CLTableFile::_s_header_size);
    _table_file->Write(headbuf,CLTableFile::_s_header_size);
    _table_file->Write(buffer,size);
    _WriteSuffix();
    _cur_row++;
    return true;
}
```

### 3.3 多线程模块实现

由于磁盘io达到了毫秒级别的时间消耗，如果以阻塞的方式让用户调用api接口，那么达到缓存上限4096字节的那一次写操作必然消耗了毫秒级别的时间。所以设定了多线程模块，该模块主要包含一个多线程池，对外提供以下接口

```c++
void Start(); //用于线程启动，文件管理模块调用
bool Submit(CLExcutiveAbstractFunc *func); //用于上层文件表模块进行读写任务的提交
bool GetFunc(CLExcutiveAbstractFunc **func);//用于线程池中每个线程进行读写任务的获取
bool ShutDown();//程序退出时关闭线程池，一般在析构函数中自动执行
```

#### 3.3.1 多线程模块类图

![threadpool](pic\threadpool.png)

### 3.3.2 核心代码

线程池中的每一个线程运行相同的逻辑，主要功能用于抢夺线程池中的任务进行执行，此处主要是读写任务

```c++
virtual void RunFuncEntity() {
    CLThreadPool *pool = (CLThreadPool *)(__runningContext);
    while (_running){
        pool->_task_submit_get_lock.Lock();
        while(pool->GetQueueSize() == 0){ //防止唤醒后，任务被其他工人抢了，导致没有任务
            pool->_has_task_condition_var.Wait(pool->_task_submit_get_lock);
        }
        if(_running){
            CLExcutiveAbstractFunc *func;
            if(pool->GetFunc(&func)){
                func->RunFuncEntity();
                _total_task_num ++;
            }else{
                std::cout<<"worker "<<_id<<" is notifyed,but can not find any task,some logic error"<<std::endl;
            }
        }else{
            //线程推出 解锁
            pool->_task_submit_get_lock.UnLock();
            break;
        }
        pool->_task_submit_get_lock.UnLock();
    }
    std::cout << "thread " << _id << " quit,total handle work number:" << _total_task_num << std::endl;
}
```

暴露给上层的线程池启动接口，主要实现各个工作线程的启动，线程池Start()逻辑代码如下

```c++
void CLThreadPool::Start() {
    for(int i=0;i<_worker_queue.size();i++){
        _worker_queue[i]->run();
    }
    _started = true;
}
```



 线程池对外暴露的提交逻辑，用于主线程提交任务

```c++
bool CLThreadPool::Submit(CLExcutiveAbstractFunc *func) {
    if(!_started){
        std::cout<<"submit a task on stoped thread pool"<<std::endl;
        return false;
    }
    CLLockGuard lockGuard(&_task_submit_get_lock);
    _task_queue.push(func);
    _has_task_condition_var.Notify_One(); //通知一个工作线程进来
    _submit_task_num++;
    return true;
}
```



### 3.4 日志模块实现

日志模块基于底层的文件模块，由于整个日志模块只存在一个，因此采用了单例的实现方式

#### 3.4.1 日志模块流程图

#### 3.4.2 核心代码

单例的GetInstance,采用DCL双检测锁实现

```c++
zy::file::CLLogger *zy::file::CLLogger::GetInstance() {
    if(_s_logger != nullptr){
        return _s_logger;
    }
    _s_create_logger_lock->Lock();
    if(_s_logger == nullptr){
        try{
            _s_logger = new CLLogger();
        } catch (std::string &errMsg) {
            _s_create_logger_lock->UnLock();
            throw errMsg;
        }
    }
    _s_create_logger_lock->UnLock();
    atexit(CLLogger::OnExit);
    return _s_logger;
}
```

日志数据写入的实现，写入时加入时间前缀和回车后缀

```c++
void zy::file::CLLogger::WriteLog(const char *msg, int total) {
    _WritePrefix();
    _logger_file->Write(msg,total);
    _WriteSuffix();
}

void zy::file::CLLogger::_WriteSuffix() {
    _logger_file->Write("\n",1);
}
void zy::file::CLLogger::_WritePrefix() {
    time_t timep;
    time(&timep);
    std::string buf(asctime(gmtime(&timep)));
    buf.back() = ':'; //替换回车为冒号
    _logger_file->Write(buf.c_str(),buf.length());
}
```

### 3.5 B树索引模块实现

B树索引模块，主要将用户的属性信息映射成文件存储的行号，该模块主要对外提供的接口如下所示

```c++
void createIndex(std::vector<int64_t> datePtr, std::vector<int> row);
bool insert(KeyType key, const DataType &data);
//范围查找，输入为上下界的key值和指示是否包含上下界的bool变量。
bool select(KeyType lowerBound, KeyType upperBound, bool lowerContain, bool upperContain, std::vector<DataType>& selectResult);
bool select(KeyType key, SELECT_TYPE selectType, std::vector<DataType>& selectResult);
```



#### 3.5.1 索引模块类图

![index](pic\index.png)

#### 3.5.2 核心代码

插入一个node节点

 ```c++
bool CLbplusTree::insertToNode(CLbplusTreeNode* node, KeyType key, const DataType& data)
{
	if (node == NULL){
		return false;
	}
	if (node->getIsLeaf()) {
		((CLbplusTreeLeafNode*)node)->insert(key, data);
	}
	else {
		CLbplusTreeInnerNode* innerNode = ((CLbplusTreeInnerNode*)node);
		int keyIndex = innerNode->getKeyIndex(key);
		int childIndex = innerNode->getChildIndex(key, keyIndex); 
		CLbplusTreeNode* childNode = innerNode->getChild(childIndex);
		if (childNode->getKeyNum() >= UPPER_BOUND_KEYNUM){
			childNode->split(node, childIndex);
			if (node->getKeyValue(keyIndex) <= key)  
			{
				childNode = innerNode->getChild(childIndex + 1);
			}
		}
		insertToNode(childNode, key, data);
	}
	return true;
}
 ```

叶子节点终端数据的插入

```c++
void CLbplusTreeLeafNode::insert(KeyType key, const DataType& data)
{
	int i;
	for (i = m_KeyNum; i >= 1 && m_KeyValues[i - 1] > key; --i){
		setKeyValue(i, m_KeyValues[i - 1]);
		setData(i, m_Datas[i - 1]);
	}
	setKeyValue(i, key);
	setData(i, data);
	setKeyNum(m_KeyNum + 1);

	if (getKeyNum() > UPPER_BOUND_KEYNUM){
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
```



### 3.6 统一用户层数据实现

统一用户层数据模块为用户提供了各种数据操作，作为用户使用的模块，提供的主要接口如下所示

```c++
struct SLReaderMethodParam{
    int row;
    std::function<void(char *buf,int size)> callback;
};
struct SLWriterMethodParam{
    char *buf;
    int size;
    std::function<void(int row,char *buf,int size)> callback;
};
bool Append(int &row,const char *buffer,int total)
bool Read(int row,char *buf,int size);
/*
* multi thread api
* */
bool MultiRead(SLReaderMethodParam *param);

bool MultiWrite(SLWriterMethodParam *param);
```

#### 3.6.1统一用户层数据实现类图

![file](pic\file.bmp)

#### 3.6.2 核心代码

append插入数据

```c++
bool Append(int &row,const char *buffer,int total){
    CLAbstractTableFile *pCLFile;

    if(!_FindCurCLFile(&pCLFile)){
        _ExtendLocalCLFile();
        _FindCurCLFile(&pCLFile);
    }
    if(pCLFile->WriteRow(buffer,total)){
        row = IncreaseRow();
    } else{
        return false;
    }
    if(_EqualMaxRow()){
        pCLFile->Flush();
    }
    return true;
}
```

read数据

```c++
bool CLFileManager::Read(int row, char *buf, int size) {
    CLAbstractTableFile *pCLFile;
    if (!_FindCLFile(row, &pCLFile)) {
        return false;
    }
    int calRow = row % (CLFileManager::_s_max_rows_for_per_file);
    calRow = ((calRow == 0) ? CLFileManager::_s_max_rows_for_per_file : calRow); //最后一行

    return pCLFile->ReadRow(calRow, buf, size);
}
```

异步读任务封装，用户进行多线程读取时，本模块提交的读任务

```c++
class CLMutilReaderFunc: public thread::CLExcutiveAbstractFunc{
    public:
    CLMutilReaderFunc(void *pContext):CLExcutiveAbstractFunc(pContext){}
    void RunFuncEntity(){
        SLReaderMethodParam *method = (SLReaderMethodParam*)(__runningContext);
        char buf[CLFileManager::GetRowSize()];
        CLFileManager::GetInstance()->Read(method->row,buf,CLFileManager::GetRowSize());
        method->callback(buf,CLFileManager::GetRowSize());
    }
    ~CLMutilReaderFunc() = default;
};
```

异步写任务封装,用户进行多线写数据时，本模块提交的写任务

```c++
class CLMutilWriterFunc: public thread::CLExcutiveAbstractFunc{
    public:
    CLMutilWriterFunc(void *pContext):CLExcutiveAbstractFunc(pContext){}
    void RunFuncEntity(){
        SLWriterMethodParam *method = (SLWriterMethodParam*)(__runningContext);
        int row = 0;
        if(CLFileManager::GetInstance()->Append(row,method->buf,method->size)){
            method->callback(row,method->buf,method->size);
        }
        delete method;
    }
    ~CLMutilWriterFunc() = default;
};
```



### 3.7统一数据构建

用户需要方便地构建100个int数据，并且快速将这100个数据变成字节流存储到文件中，本层主要提供一下接口

```c++
CLAbstractObject *CreateObject(const T &attr);
void PushBack(CLAbstractObject *p);
void ToCharBuffer(char *buf) ;
void FromCharBuffer(const char *buf);
```

#### 3.7.1数据构建实现类图

![construct](pic\construct.bmp)

3.7.2核心代码

抽象对象接口

```c++
class CLAbstractObject {
    public:
    virtual void ToCharBuffer(std::string &buf) = 0;
    virtual void ToCharBuffer(char *buf) = 0;
    virtual void FromCharBuffer(const char *buf) = 0;
    virtual void FromCharBuffer(const std::string &str) = 0;
    virtual int32_t GetBufferSize() = 0;
    virtual void Print()=0;
    virtual ~CLAbstractObject() = default;
};
```

数据对象实现

```c++
template <typename T,typename std::enable_if<std::is_pod<T>::value>* = nullptr>
    class CLDataObject: public CLAbstractObject{
        private:
        T _data;
        public:
        CLDataObject(T data){
            _data=data;
        }
        ~CLDataObject()=default;

        CLDataObject(const CLDataObject & o){
            _data = o.__data;
        }
        void ToCharBuffer(std::string &buf){
            memcpy((char *)(buf.c_str()), &_data, GetBufferSize());
        }
        void ToCharBuffer(char *buf){
            memcpy((char *)(buf), &_data, GetBufferSize());
        }
        void FromCharBuffer(const char *buf){
            memcpy(&_data, buf, GetBufferSize());
        }
        void FromCharBuffer(const std::string &buf){
            assert(buf.length()>=4);
            memcpy(&_data, buf.c_str(), GetBufferSize());
        }
        int32_t GetBufferSize(){
            return sizeof(T);
        }
        void Print(){
            std::cout<<"data:"<<_data<<" size:"<<GetBufferSize()<<std::endl;
        }
    };
```

用户构建数据的数据工厂

```c++
class CLObjectProduceFactory{
    public:
    template <typename T>
    CLAbstractObject *CreateObject(const T &attr){
        return new CLDataObject<T>(attr);
    }
};
```

用户的一整行属性数据存储

```c++
class CLUserAttrVector: public CLAbstractObject{
    private:
    static std::unordered_map<CLObjectType,size_t> _s_type_size_map;
    std::vector<CLAbstractObject *> _user_attr_vector;
    std::vector<CLObjectType> _user_attr_type;
    int _total_size=0;
    public:
    explicit CLUserAttrVector(const std::vector<CLObjectType> &type){
        _user_attr_type.reserve(type.size());
        for(auto &iter:type){
            _user_attr_type.push_back(iter);
            _total_size+=CLUserAttrVector::_s_type_size_map[iter];
        }
    }
    CLUserAttrVector(const CLUserAttrVector &) = delete;
    CLUserAttrVector &operator=(const CLUserAttrVector &) = delete;
    CLUserAttrVector(CLUserAttrVector &&o){
        for(auto &iter:o._user_attr_vector){
            o.PushBack(iter);
        }
    }
    ~CLUserAttrVector(){
        Clear();
    }
    void Clear(){
        for(auto &iter:_user_attr_vector){
            if(iter!= nullptr){
                delete iter;
                iter = nullptr;
            }
        }
    }
    void PushBack(CLAbstractObject *p){
        _user_attr_vector.push_back(p);
    }
    void ToCharBuffer(std::string &buf){
        if(buf.capacity() < GetBufferSize()){
            buf.reserve(GetBufferSize() - buf.capacity());
        }
        int offset = 0;
        for(auto &iter:_user_attr_vector){
            int bufsize =iter->GetBufferSize();
            char tempbuf[bufsize];
            iter->ToCharBuffer(tempbuf);
            memcpy((char*)(buf.data()+offset),tempbuf,bufsize);
            offset+=bufsize;
        }
    }
    void ToCharBuffer(char *buf){
        int offset = 0;
        for(auto &iter:_user_attr_vector){
            iter->ToCharBuffer(buf+offset);
            offset+=iter->GetBufferSize();
        }
    }
    void FromCharBuffer(const char *buf){
        Clear();
        int offset = 0;
        CLObjectProduceFactory factory;
        _user_attr_vector.reserve(_user_attr_type.size());
        for(auto &iter:_user_attr_type){
            if(iter == CLObjectType::Int){
                auto *p = factory.CreateObject<int>(0);
                p->FromCharBuffer(buf+offset);
                offset+=p->GetBufferSize();
                _user_attr_vector.push_back(p);
            }
        }
    }
    void FromCharBuffer(const std::string &buf){
        Clear();
        assert(buf.capacity() >= GetBufferSize());
        int offset = 0;
        for(auto &iter:_user_attr_vector){
            iter->FromCharBuffer(buf.data()+offset);
            offset+=iter->GetBufferSize();
        }
    }
    int32_t GetBufferSize(){
        if(_total_size == 0) {
            int total = 0;
            for (auto &iter:_user_attr_type) {
                total += CLUserAttrVector::_s_type_size_map[iter];
            }
            _total_size = total;
        }
        return _total_size;
    }
    void Print(){
        std::cout<<"user attr vector info:"<<" size:"<<GetBufferSize()<<std::endl;
        for(auto &iter:_user_attr_vector){
            if(iter!= nullptr){
                iter->Print();
            }
        }
    }
};
```



## 4 测试

### 4.1 基础功能测试

#### 4.1.1 create创建初始表接口测试

首先创建表结构

```c++
 //创建表结构
std::vector<CLObjectType> colums_type;
colums_type.reserve(100);
for(int i=0;i<100;i++){
    colums_type.push_back(CLObjectType::Int);
}

std::unique_ptr<zy::dms::CLUserAttrVector> userInfo[totalRow];
for(int i=0;i<totalRow;i++){
    userInfo[i].reset(new CLUserAttrVector(colums_type));
}
//初始化表，表中每个文件最多存储1000行数据，每行数据为100个int类型的数据
zy::dms::CLFileManager::InitInstance(1000,sizeof(int)*100); 
```

**插入100W行数据**

```c++
for(int i=0;i<totalRow;i++){
    auto &v = userInfo[i];
	for(int j=1;j<=100;j++){
        auto *intObject = factory.CreateObject(rand()%100);
        v->PushBack(intObject);
	}
	storage(v.get());
}
```

运行后，观察测试结果

![image-20201115232746964](/test/单线程100w.png)

通过大约96秒时间，存入了100w行数据

#### 4.1.2 read接口测试

接着上面写入的测试，随机读取100w次数据![image-20201115233518356](/test/单线程读100w.png)

#### 4.1.4  文件记录测试 

关闭程序后，理论上程序应该保存相关数据，文件记录如下所示

![image-20201115234207577](/test/文件记录测试.png)

观察meta_data.txt 观察元数据

![image-20201115234531300](/test/100w.png)

观察配置文件

![image-20201115234619661](/test/configure.png)

#### 4.1.4创建索引接口测试

索引接口目前调试依旧有问题，后期更进

### 4.3 性能测试

#### 4.3.1主线程阻塞测试

每个文件最大存储1000行数据，每行数据包含100个int，共400字节，存6666行数据效果如下图所示

**写测试**

第一次写测试结果

![image-20201115225540519](/pic/test/单线程阻塞6666.png)

第二次写测试结果

![image-20201115225727739](/pic/test/单线程66662.png)

第三次测试结果

![image-20201115225641666](/pic/test/单线程66663.png)

总结:单线程写入6666行数据需要花费时间在 （337+337+342）/3 =339ms

**读测试**

第一次读测试

![image-20201115231308420](/pic/test/单线程读6666.png)

第二次读测试

![image-20201115231335701](/pic/test/单线程读66662.png)

第三次读测试

![image-20201115231402000](/pic/test/单线程读66663.png)

经过多次测试，读6666行的数据时延在90ms左右

#### 4.3.2多线程测试

线程池开启多个个线程进行测试，每个文件最大存储1000行数据，每行数据包含100个int，共400字节，存6666行数据效果如下图所示

**双线程测试写**

![image-20201115225928856](/pic/test/2线程66661.png)

经过多次测试，存入6666行数据始终稳定在750ms左右

**四线程测试写**

![image-20201115230058819](/pic/test/4线程6666.png)

经过多次测试，存入6666行数据始终稳定在770ms左右

**六线程测试写**

![image-20201115230336634](/pic/test/6线程测试.png)

经过多次测试，存入6666行数据始终稳定在770ms左右

**双线程测试读**

![image-20201115230716077](/pic/test/2线程读6666.png)

经过多次测试，存入6666行数据始终稳定在580ms左右

**四线程测试读**

![image-20201115230826401](/pic/test/4线程读6666.png)

经过多次测试，存入6666行数据始终稳定在600ms左右

**六线程测试读**

![image-20201115230920040](/pic/test/6线程测试读.png)

经过多次测试，存入6666行数据始终稳定在640ms左右

## 5.分析和存在的问题

### **5.1多线程分析**

​	多线程并没有为系统提高效率，因为写入数据的时候，因为只支持append操作，尽管底层有多个文件，但同一时间只有一个写指针，所有线程都因为这个写入的append锁而等待，导致写数据时因多线程的锁和线程切换反而效率变低

​	值得分析的问题是，读取数据的时候为什么是随机读取，理论上不同线程去读取不同文件中的数据可以达到并行效果，为什么多线程的效率也还是下降了？？

### **5.2bug情况**

目前，多线程读取的时候，10会存在偶然的segment和double point exception，截止到目前11月15日没有时间解决。

index模块中的索引B+树 是按照之前本科的一个实现所作，插入数据存在问题，从磁盘恢复存在问题，截止到目前11月15日没有时间解决。

### 5.3值得优化的地方

对外暴露的接口太粗糙，应用使用极不方便，可以优化

b+树索引完善，从磁盘中恢复功能实现

整个代码将在后续进行完善，感谢老师一学期的教导，目前github地址为[linux 高级程序设计课程作业Github地址](https://github.com/zyProgram/Linux_Program_Toy)   url: https://github.com/zyProgram/Linux_Program_Toy

