# NS3_FuzzyTool
 Convert the fuzzy toolbox in MATLAB into an ns3 module  
 此为将matlab中的模糊工具箱修改为一个ns3模块。  
  
# Explanation:  
 The 'test', 'helper', and 'doc' folders are automatically generated when ns3 generates module   templates and have no effect.  
 The main content lies in the "fuzzy-tool.cc" and "fuzzy-tool.h" under the "model" directory, which were modified by "fis.c" of the Matlab fuzzy toolbox.
 For example usage, please refer to "examples/fuzzy-tool-example.cc". However, there are no txt files or fis files with test data attached, and users need to create them themselves. 
 The main reference is from the following website, and the usage examples are also based on the content of that website.
 其中“test”、“helper”以及“doc”文件夹是ns3生成模块模板时自动生成的，没有任何作用。  
 主要内容在于"model"目录下的"fuzzy tool.cc"和"fuzzy tool.h"，它们是由matlab模糊工具箱的fis.c修改过来的。  
 使用示例见"examples/fuzzy-tool-example.cc"，不过没有附带测试数据的txt文件以及fis文件，需要用户自行创建。  
 主要参考自下述网址，使用示例也是基于该网址内容的。
  
# Refer to:  
 <https://blog.csdn.net/qq_40261137/article/details/104321031>  


