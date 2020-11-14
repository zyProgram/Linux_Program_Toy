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

### 2.2 总体设计框图

### 2.3 各模块简介

 

## 3 详细设计与实现

本章节主要介绍代码的主要功能实现逻辑，首先介绍了本次代码的书写规范，然后采用了代码实现的时间先后，采用自下而上的封装方式叙述整个代码各个模块的详细实现。

### 3.1代码规范

在介绍代码设计之前，由于代码不是一次性书写完成，前后周期跨度约为一个月，为了让代码更加整洁，开发方便。首先需要约定好代码规范。在代码实现时，使用本学期李林老师课上常用代码规范，结合自身比较熟练的代码标准，现将本程序所考虑到的代码规范规约如下

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



#### 3.2.1 底层文件模块流程

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

### 3.3 表文件数据读写模块

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

#### 3.3.1表文件数据读写流程图

#### 3.3.3 核心代码

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

### 3.4 多线程模块实现

由于磁盘io达到了毫秒级别的时间消耗，如果以阻塞的方式让用户调用api接口，那么达到缓存上限4096字节的那一次写操作必然消耗了毫秒级别的时间。所以设定了多线程模块，该模块主要包含一个多线程池，对外提供以下接口

```c++
void Start(); //用于线程启动，文件管理模块调用
bool Submit(CLExcutiveAbstractFunc *func); //用于上层文件表模块进行读写任务的提交
bool GetFunc(CLExcutiveAbstractFunc **func);//用于线程池中每个线程进行读写任务的获取
bool ShutDown();//程序退出时关闭线程池，一般在析构函数中自动执行
```



#### 3.4.1 多线程模块流程图

### 3.4.2 核心代码

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



### 3.5 日志模块实现

日志模块基于底层的文件模块，由于整个日志模块只存在一个，因此采用了单例的实现方式

#### 3.5.1 日志模块流程图

#### 3.5.2 核心代码

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

### 3.6 B树索引模块实现

#### 3.6.1 索引模块流程图

#### 3.6.2 核心代码

 

### 3.7 统一用户层数据实现

统一用户层数据模块为用户提供了各种数据操作，作为用户使用的模块

#### 3.7.1 统一数据接口流程图

#### 3.7.2 核心代码

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

插入1000行数据

```c++
for(int i=0;i<1000;i++){
    auto &v = userInfo[i];
	for(int j=1;j<=100;j++){
        auto *intObject = factory.CreateObject(rand()%100);
        v->PushBack(intObject);
	}
	storage(v.get());
}
```

运行后，观察测试结果

#### 4.1.2 read接口测试

接着上面写入的测试，尝试读出写入的一行数据

```c++
int size = 400;
char buf[size];
zy::dms::CLFileManager::GetInstance()->Read(1994,buf,size);
CLUserAttrVector desVec(colums_type);
desVec.FromCharBuffer(buf);
```

#### 4.1.3 创建索引接口测试

#### 4.1.4 文件记录测试

### 4.2 异常测试

#### 4.2.1 开关系统测试

关闭

### 4.3 性能测试

#### 4.3.1单线程测试

#### 4.3.2多线程测试

 

 
