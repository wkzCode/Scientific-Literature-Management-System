#include"count.h"

//互斥锁
mutex mtx; 

//作者文章数统计
wxString article_count()
{
    wxString result;
    result = "写文章最多的前100名作者为: \n";
    vector<author_articleNum>counts;
    int size = 0;

    for (auto it = authors.begin(); it != authors.end(); ++it)
    {
        int n = author_article.count(*it);
        if (n > 1)
        {
            int len = counts.size();
            author_articleNum new_element(*it, n);
            if (counts.size() == 0)
                counts.push_back(new_element);
            //大小设为100，插入排序
            else if (len < 100)
            {
                bool result = false;
                for (int i = 0; i < len; i++)
                {
                    if (counts[i].n < n)
                    {
                        counts.insert(counts.begin() + i, new_element);
                        result = true;
                        break;
                    }
                }
                if (!result)
                    counts.push_back(new_element);
            }
            //元素超过100后，对大于第100个元素的值插入排序
            else
            {
                if (n > counts[99].n)
                {
                    for (int i = 0; i < 100; i++)
                    {
                        if (counts[i].n < n)
                        {
                            counts.insert(counts.begin() + i, new_element);
                            break;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < 100; i++)
        result += wxString::Format("- %s\n", counts[i].str + ": " + to_string(counts[i].n) + "篇");

    return result;
}

//显示有文章发表的所有年份
wxString show_years()
{
    wxString result;
    result = "有文章发表的所有年份有: \n";
    for (auto it = years.begin(); it != years.end(); it++)
    {
        result += wxString::Format("- %s\n", *it);
    }
    return result;
}

//关键词频率统计
wxString keywords_count(wxString year)
{
    wxString result;
    result = "该年文章标题中出现频率排名前10的关键词为: \n";
    vector<string>articles;
    auto range = year_article.equal_range(year.ToStdString());
    if (range.first != year_article.end())
    {
        unordered_map<string, int>word_sequence;
        // 处理多篇文章的情况
        for (auto it = range.first; it != range.second; ++it)
        {
            string sentence = it->second.substr(0, it->second.length() - 2);
            stringstream ss(sentence);
            string word;
            vector<string> words;

            // 以空格为分隔符提取单词
            while (ss >> word) 
            {
                if (word != "of" && word != "A"&& word != "a"&& word != "to"&& word != "the"&& word != "is"
                    && word != "how"&& word != "Why"&& word != "and"&&word != "on"&& word != "for"&&
                    word != "in"&& word != "The"&& word != "with"&& word != "an"&& word != "by"&&
                    word != "On"&& word != "An"&& word != "-"&& word != "With"&& word != "using"&& word != "Using")
                    words.push_back(word);
            }
            for(int i=0; i<words.size(); i++)
            {
                auto it2 = word_sequence.find(words[i]);
                if(it2!=word_sequence.end())
                    it2->second++;
                else
                    word_sequence.emplace(words[i], 1);
            }
        }

        vector<pair<string, int>>maxwords;
        for (auto it = word_sequence.begin(); it != word_sequence.end(); ++it)
        {
            int len = maxwords.size();
            if (len == 0)
            {
                maxwords.push_back(*it);
            }
            else if(len<10)
            {
                bool result = false;

                for (int i = 0; i < len; i++)
                {
                    if (maxwords[i].second < it->second)
                    {
                        maxwords.insert(maxwords.begin() + i, *it);
                        result = true;
                        break;
                    }
                }
                if (!result)maxwords.push_back(*it);
            }
            else
            {
                if (it->second > maxwords[9].second)
                {
                    for (int i = 0; i < 10; i++)
                    {
                        if (maxwords[i].second < it->second)
                        {
                            maxwords.insert(maxwords.begin() + i, *it); break;
                        }
                    }
                }
            }
        }

        for (int i = 0; i < 10; i++)
            result += wxString::Format("- %s\n", maxwords[i].first + ": " + to_string(maxwords[i].second) + "次");
    }
    else
        result = wxString::Format("未找到该年发表的文章 %s！", year);

    return result;
}

//各阶完全子图统计
wxString graph_count()
{
    mutex mtx; // 互斥锁
    wxString result="各阶完全子图统计如下：\n";
    map<int, int> completeSubgraphCounts; // 用于存储各阶完全子图的数量
    vector<future<void>> futures;
    unordered_set<string>coauthors;
    int size = 0;

    //遍历作者并启动异步线程寻找完全子图
    for(auto it =authors.begin(); it !=authors.end();++it)
    {
        if (coauthors.find(*it) == coauthors.end())
        {
            unordered_set<string>this_coauthors = find_coauthors(*it);
            size = this_coauthors.size();
            coauthors.insert(this_coauthors.begin(), this_coauthors.end());
            if (size > 0)
            {
                futures.emplace_back(
                    async(launch::async, [&completeSubgraphCounts, &mtx, this_coauthors]() 
                    {
                        vector<vector<int>> graph = find_all_graph(this_coauthors);
                        BronKerboschIterative(graph, completeSubgraphCounts);
                    }));
            }
        }
    }
    
    // 等待所有任务完成
    for (auto&& future : futures)
    {
        future.get();
    }

    //展示结果
    for (auto it=completeSubgraphCounts.begin();it!=completeSubgraphCounts.end();it++) 
    {
        result += wxString::Format("- %s\n", to_string(it->first) + "阶: " + to_string(it->second) + "次");
    }
    return result;
}
//生成邻接矩阵
vector<vector<int>> find_all_graph(unordered_set<string>coauthors)
{
    vector<string>v_coauthors(coauthors.begin(),coauthors.end());
    int size=v_coauthors.size();
    vector<vector<int>>graph(size, vector<int>(size));

    for (int i = 0; i < size; i++)
    {
        unordered_set<string>c_coauthors = find_coauthors(v_coauthors[i]);
        vector<string>vc_coauthors(c_coauthors.begin(), c_coauthors.end());

        for(int j=0;j<vc_coauthors.size();j++)
        {
            //先判断是否有再进行遍历
            if (coauthors.find(vc_coauthors[j]) != coauthors.end())
            {
                for (int k = 0; k < size; k++)
                {
                    if (vc_coauthors[j] == v_coauthors[k])
                    {
                        graph[i][k] = 1;
                        graph[k][i] = 1;
                        break;
                    }
                }
            }
        }
    }

    return graph;    
}
//使用Bron-Kerbosch算法寻找完全子图
void BronKerboschIterative(const vector<vector<int>>& graph, map<int, int>& completeSubgraphCounts) 
{
    stack<BronKerboschState> stk;//防止直接使用递归因递归过深爆栈
    BronKerboschState initialState;
    for (int i = 0; i < graph.size(); i++) // 初始化P为图中的所有顶点
        initialState.P.push_back(i);
    stk.push(initialState);
    while (!stk.empty()) 
    {
        BronKerboschState current = stk.top();
        stk.pop();
        if (current.P.empty() && current.X.empty()) 
        {
            int cliqueSize = current.R.size();
            if (cliqueSize > 0) 
            {
                lock_guard<mutex> lock(mtx); // 使用互斥锁保护共享资源
                completeSubgraphCounts[cliqueSize]++;
            }
        }
        else 
        {
            int pivot = current.P.empty() ? -1 : current.P[0];
            if (pivot != -1) 
            {
                auto it = current.P.begin();
                while (it != current.P.end()) 
                {
                    int v = *it;
                    if (graph[pivot][v] == 1) // 如果与pivot相连，跳过
                    {
                        ++it;continue;
                    }
                    BronKerboschState newState;
                    newState.R = current.R;
                    newState.R.push_back(v); // 将v添加至R
                    // 新P是当前P中所有与v相连的顶点
                    for (int w : current.P) if (graph[v][w] == 1) newState.P.push_back(w);
                    // 新X是当前X中所有与v相连的顶点
                    for (int w : current.X) if (graph[v][w] == 1) newState.X.push_back(w);
                    stk.push(newState);
                    // 从P中移除v并将v添加到X，以避免后续重复访问
                    it = current.P.erase(it);
                    current.X.push_back(v);
                }
            }
        }
    }
}