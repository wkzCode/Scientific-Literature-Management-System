#pragma once
#include"common_use.h"
#include"search.h"

//作者姓名及文章数
struct author_articleNum
{
    string str = "";
    int n = 0;
    author_articleNum(string s, int num)
    {
        str = s; n = num;
    }
};

//作者文章数统计
wxString article_count();

//显示有文章发表的所有年份
wxString show_years();

//关键词频率统计
wxString  keywords_count(wxString year);

//各阶完全子图统计
wxString graph_count();
//生成邻接矩阵
vector<vector<int>> find_all_graph(unordered_set<string>coauthors);
//使用Bron-Kerbosch算法寻找完全子图
void BronKerboschIterative(const vector<vector<int>>& graph, map<int, int>& completeSubgraphCounts);
//存储各种顶点
struct BronKerboschState
{
    vector<int> R;
    vector<int> P;
    vector<int> X;
};