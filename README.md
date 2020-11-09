# Linux_Program_Toy
# Linux_Program_Toy

# Linux程序设计实现

## 1 需求分析

### 1.1 课程基本功能需求

本次Linux高级程序设计的基本要求是需要实现一个数据表管理系统，该系统能够存储一张数据表，数据表中每个数据的属性数量都有100个，所支持的存储数据条目数量不少于100万条，数据表最终需要存储在磁盘上。

按照课程功能，拟定需要从接口，存储，索引，并发四个方面进行总体需求分析。

### 1.2 接口功能需求

按照课程基本要求，整个系统管理了一张数据表，该系统需要提供基础Create(创建),Retrieve(查询),Update(更新),Delete(删除)功能，以接口形式暴露，简称CRUD接口。除此之外，由于数据量过于庞大，需要建立索引机制，那么系统还需要提供从特定属性建立索引的功能，该功能需要通过一个建立索引接口暴露给上层。

### 1.3 存储功能需求

整个系统应能够存储不少于100w行的文件数据。大量的数据不可能一直处在内存中，文件数据需要能够dump到磁盘进行存储。除此之外，索引文件在程序结束时也需要能够存储在内存中，

### 1.4 索引需求

按照上述要求描述，必然需要提供索引支持数据的查询操作，索引能够根据用户数据中100个属性中的某一个属性构建，可参考采用b树或者b+树实现。

### 1.5 并发需求

在磁盘进行io时，io处理延时较高，内部采用多线程模型进行数据的写入。

 

## 2 总体设计

### 2.1 按照需求，总体设计

### 2.2 总体设计框图

 

## 3 详细设计与实现

### 3.1 底层文件模块实现

#### 3.1.1 文件模块流程图

#### 3.1.2 核心代码

### 3.2 多线程模块实现

#### 3.2.1 多线程模块流程图

#### 3.2.2 核心代码

 

### 3.3 日志模块实现

#### 3.3.1 多线程模块流程图

### 3.3.2 核心代码

#### 3.4 B树索引模块实现

#### 3.4.1 索引模块流程图

#### 3.4.2 核心代码

 

### 3.5 统一用户层数据实现

#### 3.5.1 统一数据实现

#### 3.5.2 核心代码

## 4 测试

### 4.1 基础功能测试

#### 4.1.1 create接口测试

#### 4.1.2 retrieve接口测试

#### 4.1.3 update接口测试

#### 4.1.4 创建索引接口测试

### 4.2 异常测试

#### 4.2.1 开关系统后

### 4.3 性能测试

 

 