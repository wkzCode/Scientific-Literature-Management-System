#include"Data_initialize.h"

//定义映射关系
//标题到作者的映射
unordered_map<string, vector<string>>article_authors;
//作者到标题的映射
unordered_multimap<string, string>author_article;
//年份到标题的映射
unordered_multimap<string, string>year_article;

//定义集合
//所有作者集合
unordered_set<string> authors;
//存储所有年份的集合
unordered_set<string> years;

//互斥锁，确保能够同时读入两个映射关系
mutex mapMutex;
mutex mapMutex1;

//分割映射文件的字符串
int split(string str)
{
    int len = str.length(), i = 0;
    for (; i < len; i++)
    {
        if (str[i] == '@')break;
    }
    return i;
}

//加载作者标题映射关系
void load_map(string str)
{
    lock_guard<mutex> lock(mapMutex);
    string filename = str + ".txt";
    //fstream inFile(filename, ios::in);
    if (str == "article_authors")
    {
        fstream inFile(filename, ios::in);
        while (!inFile.eof())
        {
            string str, title, author;
            getline(inFile, str);

            int index = split(str);
            if (index)
            {
                title = str.substr(0, index);
                author = str.substr(index + 1);
                auto it = article_authors.find(title);
                if (it == article_authors.end())
                {
                    vector<string>authors;
                    authors.push_back(author);
                    article_authors.emplace(title, authors);
                }
                else
                {
                    it->second.push_back(author);
                }
            }
        }
        inFile.close();
    }
    else if (str == "author_article")
    {
        fstream inFile(filename, ios::in);
        while (!inFile.eof())
        {
            string str, title, author;
            getline(inFile, str);
            int index = split(str);
            if (index)
            {
                author = str.substr(0, index);
                title = str.substr(index + 1);
                author_article.emplace(author, title);
            }
        }    
        inFile.close();
    }
}

//加载年份与标题的映射关系
void load_yearmap()
{
    lock_guard<mutex> lock(mapMutex1);
    string filename = "year_article.txt";
    fstream inFile(filename, ios::in);
    while (!inFile.eof())
    {
        string str, title, year;
        getline(inFile, str);
        if (str[0] != '@' && str.length() > 5)
        {
            year = str.substr(0, 4);
            title = str.substr(5);
            year_article.emplace(year, title);
        }
    }
    inFile.close();
}

//生成作者集合
void load_authors()
{
    for (auto it = author_article.begin(); it != author_article.end(); ++it)
    {
        authors.insert(it->first);
    }
}

//生成年份集合
void load_years()
{
    for (auto it = year_article.begin(); it != year_article.end(); ++it)
    {
        years.insert(it->first);
    }
}

//加载判断
wxString judge_load()
{
    if (article_authors.size() == 0)
        return "映射文件未加载！";
    else return "";
}