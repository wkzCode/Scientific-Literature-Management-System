#pragma once
#include"common_use.h"

//枚举xml文件信息类型
enum xml_information
{
    article_mdate, key, author, title, journal, volume, month, year, cdrom, ee
};

//xml结构体
struct xml
{
    int sequence=0;
    string xml_label, article_mdate, key, title, journal, volume, month, year, cdrom, ee;
    vector<string>authors;
};

//判断xml信息类型
xml_information judge_xml_information(string str);

//提取xml标签内的值并赋值
void xml_abstract(string str, xml_information type, xml* x);

//获取article_mdate值
int get_article_mdate(string str);

//获取key的值
string get_key(string str);

//读取xml文件
void readfile();

//将映射信息写入文件
void save_to_file(string str, xml element);