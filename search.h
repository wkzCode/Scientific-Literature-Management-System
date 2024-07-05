#pragma once
#include"common_use.h"
#include"xml_abstract.h"
#include"Data_initialize.h"

//根据作者查找文章信息
wxString find_by_author(wxString authorName);

//查找特定文章信息并输出
wxString find_specific_article(wxString articleName);

//查找与目标作者的合作者并输出
unordered_set<string> find_coauthors(string authorName);
wxString output_coauthors(wxString authorName);

//查找包含关键词的文章并输出
wxString find_by_keywords(wxString keywords);