#include"search.h"

//根据作者查找文章信息
wxString find_by_author(wxString authorName)
{
    wxString result;
    // 调用查找函数
    auto range = author_article.equal_range(authorName.ToStdString());
    if (range.first != author_article.end())
    {
        // 处理多篇文章的情况
        result = "作者 " + authorName + " 对应的文章有：\n";
        for (auto it = range.first; it != range.second; ++it)
        {
            result += wxString::Format("- %s\n", it->second+"\n");
        }
    }
    else
    {
        result = wxString::Format("未找到作者 %s！", authorName);
    }
    return result;
}

//查找特定文章信息并输出
wxString find_specific_article(wxString articleName)
{
    wxString wxresult;
    string str=articleName.ToStdString();
    string s = "<title>" + str + "</title>";
    ifstream file;
    file.open("D:/GUI/dblp.xml", ios::in);
    string content;
    string unknown_str;
    xml element;
    bool result = false;
    while (!file.eof())
    {
        getline(file, unknown_str);
        xml_information xml_type = judge_xml_information(unknown_str);

        if (xml_type == article_mdate)
        {
            xml new_element;
            //读取标签信息
            xml_abstract(unknown_str, article_mdate, &new_element);
            int begin = get_article_mdate(unknown_str);
            new_element.article_mdate = new_element.xml_label.substr(begin, 10);
            new_element.key = get_key(new_element.xml_label.substr(begin));

            getline(file, unknown_str);

            //读取其他信息
            while (unknown_str.find("</article>") == string::npos)
            {
                xml_type = judge_xml_information(unknown_str);
                xml_abstract(unknown_str, xml_type, &new_element);
                if (xml_type == title)
                {
                    if (new_element.title != str)break;
                    else result = true;
                }
                getline(file, unknown_str);
            }
            if (result == true)
            {
                element = new_element;
                break;
            }
        }
    }
    if (result == true)
    {
        wxresult = "文章 " + articleName + " 的全部信息如下: \n";
        wxresult+=wxString::Format("-article mdate : %s\n",element.article_mdate);
        wxresult+=wxString::Format("-key : %s\n",element.key);
        for (int i = 0; i < element.authors.size(); i++)
        {
            wxresult+=wxString::Format("-author : %s\n",element.authors[i]);
        }
        wxresult+=wxString::Format("-title : %s\n",element.title);
        wxresult+=wxString::Format("-journal : %s\n",element.journal);
        wxresult+=wxString::Format("-volume : %s\n",element.volume);
        wxresult+=wxString::Format("-month : %s\n",element.month);
        wxresult+=wxString::Format("-year : %s\n",element.year);
        wxresult+=wxString::Format("-cdrom : %s\n",element.cdrom);
        wxresult+=wxString::Format("-ee : %s\n",element.ee);
    }
    else
    {
        wxString inf = "未找到" + articleName + " %s！";
        wxresult = wxString::Format(inf);
    }
    file.close();
    return wxresult;
}

//查找目标作者的合作者
unordered_set<string>find_coauthors(string authorName)
{
    vector<string>articles;
    unordered_set<string> authors;

    //unordered_multimap的遍历
    auto range = author_article.equal_range(authorName);
    for (auto it = range.first; it != range.second; ++it)
    {
        auto it2 = article_authors.find(it->second);
        if (it2 != article_authors.end())
        {
            for (int i = 0; i < it2->second.size(); i++)
            {
                if (it2->second[i] != authorName)
                    authors.insert(it2->second[i]);
            }
        }
    }
    return authors;
}

//查找目标作者的合作者并输出
wxString output_coauthors(wxString authorName)
{
    wxString result;
    unordered_set<string> authors=find_coauthors(authorName.ToStdString());

    //输出合作者
    if(authors.size())
    {
        result = "作者 " + authorName + " 的合作者有：\n";
        for (unordered_set<string>::iterator it = authors.begin(); it != authors.end(); ++it)
            result+=wxString::Format("- %s\n", *it);
    }
    else
    {
        result = "未找到" + authorName + "的合作者";
    }
    return result;
}

//查找包含关键词的文章并输出
wxString find_by_keywords(wxString keywords)
{
    wxString result="包含关键词的文章有：\n";
    stringstream ss(keywords.ToStdString());
    string word;
    vector<string> words;
    bool b = false;

    // 以空格为分隔符提取单词
    while (ss >> word) 
    {
        words.push_back(word);
    }

    for(auto it=article_authors.begin();it!=article_authors.end();++it)
    {
        for(int j=0;j<words.size();j++)
        {
            if(it->first.find(words[j])!=string::npos)
            {
                result += wxString::Format("- %s\n", it->first+"\n");
                b=true;
            }
        }
    }
    if(!b)
        result="没有包含给定关键词的文章";

    return result;
}