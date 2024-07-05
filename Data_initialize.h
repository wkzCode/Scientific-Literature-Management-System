#include"common_use.h"

//定义映射关系
//标题到作者的映射
extern unordered_map<string, vector<string>>article_authors;
//作者到标题的映射
extern unordered_multimap<string, string>author_article;
//年份到标题的映射
extern unordered_multimap<string, string>year_article;

//定义集合
//所有作者集合
extern unordered_set<string> authors;
//存储所有年份的集合
extern unordered_set<string> years;

//分割映射文件的字符串
int split(string str);

//加载作者标题映射关系
void load_map(string str);

//加载年份与标题的映射文件
void load_yearmap();

//生成作者集合
void load_authors();

//生成年份集合
void load_years();

//加载判断
wxString judge_load();