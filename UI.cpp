#include"UI.h"

wxIMPLEMENT_APP(MyApp);


// 在相同的头文件或源文件中，使用这个宏定义上述声明的事件类型
wxDEFINE_EVENT(wxEVT_COMMAND_MY_UPDATE_PROGRESS, wxThreadEvent);

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(): wxFrame(NULL, wxID_ANY, "科研文献管理系统")
{
    //file按钮
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_LoadFile, "&加载映射文件\tCtrl-H", "从txt中加载作者与文章标题的互相映射关系");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    //search按钮
    wxMenu* menuSearch = new wxMenu;
    menuSearch->Append(ID_Search_author_to_article,"&查找作者对应的文章信息","从映射关系中找到作者对应的文章标题");
    menuSearch->Append(ID_Search_article, "&显示目标论文的所有信息","从原文件中查找该论文对应的所有信息");
    menuSearch->Append(ID_Search_coauthor, "&查找目标作者的所有合作作者","通过两次映射找到所有合作者");
    menuSearch->Append(ID_Search_keywords, "&查找关键词对应文章信息","查找题目中包含该关键字的文章信息");

    //count按钮
    wxMenu*menuCount = new wxMenu;
    menuCount->Append(ID_Count_authors, "&统计文章数排名前100的作者","统计文章数前100名作者");
    menuCount->Append(ID_Count_keywords, "&统计频率前10的关键词","统计频率前10的关键词");
    menuCount->Append(ID_Count_graphs, "&统计各阶完全子图个数","统计聚团分析中各阶完全子图个数");

    //show按钮
    wxMenu* menuShow = new wxMenu;
    menuShow->Append(ID_Show_years, "&显示有文章发表的所有年份", "显示有文章发表的所有年份");
    menuShow->Append(ID_Show_graphs, "&显示作者之间合作关系图及其相关文章信息","显示作者之间合作关系图及其相关文章信息");

    //help按钮
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    //按钮菜单
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&文件");
    menuBar->Append(menuSearch,"&查找");
    menuBar->Append(menuCount,"&统计");
    menuBar->Append(menuShow,"&显示");
    menuBar->Append(menuHelp, "&帮助");
    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("欢迎使用!");

    //将按钮与函数联系起来
    Bind(wxEVT_MENU, &MyFrame::OnLoadFile, this, ID_LoadFile);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MyFrame::OnSearch_author_to_article, this,ID_Search_author_to_article);
    Bind(wxEVT_MENU,&MyFrame::OnSearch_article,this,ID_Search_article);
    Bind(wxEVT_MENU,&MyFrame::OnSearch_coauthors,this,ID_Search_coauthor);
    Bind(wxEVT_MENU, &MyFrame::OnSearch_keywords, this, ID_Search_keywords);
    Bind(wxEVT_MENU,&MyFrame::OnCount_authors,this,ID_Count_authors);
    Bind(wxEVT_MENU,&MyFrame::OnCount_graphs,this,ID_Count_graphs);
    Bind(wxEVT_MENU,&MyFrame::OnCount_keywords,this,ID_Count_keywords);
    Bind(wxEVT_MENU, &MyFrame::OnShow_years, this, ID_Show_years);
    Bind(wxEVT_MENU, &MyFrame::OnShow_graphs, this, ID_Show_graphs);
    Bind(wxEVT_COMMAND_MY_UPDATE_PROGRESS, &MyFrame::OnUpdateProgress, this);
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxString wxstr = "这是一个科研文献管理系统的例子，能够实现的功能如下：\n";
    wxstr += wxString::Format("- %s\n", "F1. 基本搜索功能。输入作者名，能展示该作者发表的所有论文信息。\n输入完整的论文的题目，能展示该论文的其他相关信息");
    wxstr+= wxString::Format("- %s\n", "F2. 相关搜索。输入作者名，能展示于该作者有合作关系的其他所以作者。");
    wxstr+=wxString::Format("- %s\n","F3. 作者统计功能。输出写文章最多的前100名作者。");
    wxstr+=wxString::Format("- %s\n","F4. 热点分析功能。分析每一年发表的文章中，题目所包含的单词中，出现频率排名前10的关键词。");
    wxstr+=wxString::Format("- %s\n","F5. 部分匹配搜索功能。给定若干个关键字，能快速搜索到题目中包含该关键字的文章信息");
    wxstr+=wxString::Format("- %s\n","F6. 聚团分析。作者之间的合作关系可以看成是一个图，能统计整个图中各阶完全子图的个数。");
    wxstr+=wxString::Format("- %s\n","F7. 可视化显示。通过图形化界面，展示作者之间合作关系图及其相关文章信息。");
    wxMessageBox(wxstr, "关于科研文献管理系统", wxOK | wxICON_INFORMATION);
}

//加载映射关系及作者集合
void MyFrame::OnLoadFile(wxCommandEvent& event)
{
    wxString result = judge_load();
    if (result == "")
    {
        wxMessageBox("已加载文件", "提示", wxOK | wxICON_INFORMATION);
    }
    else
    {
        wxMessageBox("开始加载映射文件", "提示", wxOK | wxICON_INFORMATION);

        // 定义异步任务的参数
        vector<string> messages = { "article_authors", "author_article" };

        // 存储异步任务的 vector<future>
        vector<future<void>> asyncTasks;

        // 启动两个没有返回值的异步任务，并传递参数
        for (const auto& message : messages)
        {
            asyncTasks.push_back(async(launch::async, load_map, message));
        }

        asyncTasks.push_back(async(launch::async, load_yearmap));

        // 在等待所有异步任务完成的同时，可以在主线程中执行其他工作
        while (any_of(asyncTasks.begin(), asyncTasks.end(), [](const future<void>& task)
            {
                return task.wait_for(chrono::milliseconds(100)) != future_status::ready;
            }))
        {
            // 在等待的过程中可以执行其他工作
            wxYield();
        }

        load_authors();
        load_years();

        wxMessageBox("映射文件及作者集合加载完成", "提示", wxOK | wxICON_INFORMATION);
    }
}

// 通过作者查找文章名
void MyFrame::OnSearch_author_to_article(wxCommandEvent& event)
{
    wxString result = judge_load();
    if (result != "")
    {
        wxMessageBox(result, "提示", wxOK | wxICON_INFORMATION);
    }
    else
    {
        wxString defaultText = "";
        wxString prompt = "请输入作者姓名:";
        wxString caption = "查找该作者发表的全部文章的标题名";

        // 创建对话窗口
        wxTextEntryDialog dialog(this, prompt, caption, defaultText, wxOK | wxCANCEL);

        // 如果输入信息则调用查找函数
        if (dialog.ShowModal() == wxID_OK)
        {
            wxString authorName = dialog.GetValue();
            result = find_by_author(authorName);

            // 在GUI中显示结果
            wxMessageBox(result, "查找结果", wxOK | wxICON_INFORMATION);
        }
    }
}

//通过文章名查找文章全部信息
void MyFrame::OnSearch_article(wxCommandEvent& event)
{
    wxString defaultText = "";
    wxString prompt = "请输入完整的文章标题:";
    wxString caption = "查找该文章全部信息";

    //创建对话窗口
    wxTextEntryDialog dialog(this, prompt, caption, defaultText, wxOK | wxCANCEL);

    //如果输入信息则调用查找函数
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString articleName = dialog.GetValue();
        //wxString result=find_specific_article(articleName);

        // 使用 async 启动异步任务
        auto task = async(launch::async, [&]() {return find_specific_article(articleName); });

        // 在等待异步任务完成的同时，可以在主线程中执行其他工作
        while (task.wait_for(chrono::milliseconds(100)) != future_status::ready)
        {
            // 在等待的过程中可以执行其他工作
            wxYield();
        }

        // 获取异步任务的返回值
        wxString result = task.get();

        // 在GUI中显示结果
        wxMessageBox(result, "查找结果", wxOK | wxICON_INFORMATION);
    }
}

//查找合作者
void MyFrame::OnSearch_coauthors(wxCommandEvent& event)
{
    wxString result = judge_load();
    if (result != "")
    {
        wxMessageBox(result, "提示", wxOK | wxICON_INFORMATION);
    }
    else
    {
        wxString defaultText = "";
        wxString prompt = "请输入作者姓名:";
        wxString caption = "查找该作者的合作者";

        //创建对话窗口
        wxTextEntryDialog dialog(this, prompt, caption, defaultText, wxOK | wxCANCEL);

        //如果输入信息则调用查找函数
        if (dialog.ShowModal() == wxID_OK)
        {
            wxString authorName = dialog.GetValue();
            result = output_coauthors(authorName);

            // 在GUI中显示结果
            wxMessageBox(result, "查找结果", wxOK | wxICON_INFORMATION);
        }
    }
}

//查找关键词对应文章信息
void MyFrame::OnSearch_keywords(wxCommandEvent& event)
{
    wxString result = judge_load();
    if (result != "")
    {
        wxMessageBox(result, "提示", wxOK | wxICON_INFORMATION);
    }
    else
    {
        wxString defaultText = "如果关键词有多个请用空格隔开";
        wxString prompt = "请输入关键词:";
        wxString caption = "查找关键字对应文章信息";

        //创建对话窗口
        wxTextEntryDialog dialog(this, prompt, caption, defaultText, wxOK | wxCANCEL);

        //如果输入信息则调用查找函数
        if (dialog.ShowModal() == wxID_OK)
        {
            wxString keywords = dialog.GetValue();
            // 启动异步线程
            future<wxString> resultFuture = async(launch::async, find_by_keywords, keywords);

            // 在等待异步任务完成的同时，可以在主线程中执行其他工作
            while (resultFuture.wait_for(chrono::milliseconds(100)) != future_status::ready)
            {
                // 在等待的过程中可以执行其他工作
                wxYield();
            }

            // 获取异步任务的返回值
            result = resultFuture.get();

            // 在GUI中显示结果
            wxMessageBox(result, "查找结果", wxOK | wxICON_INFORMATION);
        }
    }
}

//统计文章数排名前100的作者
void MyFrame::OnCount_authors(wxCommandEvent&event)
{
    wxString result = judge_load();
    if (result != "")
    {
        wxMessageBox(result, "提示", wxOK | wxICON_INFORMATION);
    }
    else
    {
        // 启动异步线程
        future<wxString> resultFuture = async(launch::async, article_count);

        // 在等待异步任务完成的同时，可以在主线程中执行其他工作
        while (resultFuture.wait_for(chrono::milliseconds(100)) != future_status::ready)
        {
            // 在等待的过程中可以执行其他工作
            wxYield();
        }

        // 获取异步任务的返回值
        result = resultFuture.get();
        // 在GUI中显示结果
        wxMessageBox(result, "统计结果", wxOK | wxICON_INFORMATION);
    }
}

//显示有文章发表的所有年份
void MyFrame::OnShow_years(wxCommandEvent& event)
{
    wxString result = judge_load();
    if (result != "")
    {
        wxMessageBox(result, "提示", wxOK | wxICON_INFORMATION);
    }
    else
    {
        // 启动异步线程
        future<wxString> resultFuture = async(launch::async, show_years);

        // 在等待异步任务完成的同时，可以在主线程中执行其他工作
        while (resultFuture.wait_for(chrono::milliseconds(100)) != future_status::ready)
        {
            // 在等待的过程中可以执行其他工作
            wxYield();
        }

        // 获取异步任务的返回值
        result = resultFuture.get();

        // 在GUI中显示结果
        wxMessageBox(result, "统计结果", wxOK | wxICON_INFORMATION);
    }
}

//统计频率前10的关键词
void MyFrame::OnCount_keywords(wxCommandEvent& event)
{
    wxString result = judge_load();
    if (result != "")
    {
        wxMessageBox(result, "提示", wxOK | wxICON_INFORMATION);
    }
    else
    {
        wxString defaultText = "";
        wxString prompt = "请输入年份:";
        wxString caption = "统计该年份标题中出现频率前10的单词";

        // 创建对话窗口
        wxTextEntryDialog dialog(this, prompt, caption, defaultText, wxOK | wxCANCEL);

        // 如果输入信息则调用查找函数
        if (dialog.ShowModal() == wxID_OK)
        {
            wxString year = dialog.GetValue();
            result = keywords_count(year);

            // 在GUI中显示结果
            wxMessageBox(result, "查找结果", wxOK | wxICON_INFORMATION);
        }
    }
}

//统计各阶完全子图个数
void MyFrame::OnCount_graphs(wxCommandEvent& event)
{
    wxString result = judge_load();
    if (result != "")
    {
        wxMessageBox(result, "提示", wxOK | wxICON_INFORMATION);
    }
    else
    {
        // 启动异步线程
        future<wxString> resultFuture = async(launch::async, graph_count);

        // 在等待异步任务完成的同时，可以在主线程中执行其他工作
        while (resultFuture.wait_for(chrono::milliseconds(100)) != future_status::ready)
        {
            // 在等待的过程中可以执行其他工作
            wxYield();
        }

        // 获取异步任务的返回值
        result = resultFuture.get();

        // 在GUI中显示结果
        wxMessageBox(result, "统计结果", wxOK | wxICON_INFORMATION);
    }
}

//显示作者之间合作关系图及其相关文章信息
void MyFrame::OnShow_graphs(wxCommandEvent& event) 
{
    wxString result = judge_load();
    if (result != "")
    {
        wxMessageBox(result, "提示", wxOK | wxICON_INFORMATION);
    }
    else
    {
        wxDialog* dialog = new wxDialog(this, wxID_ANY, wxT("作者关系展示"), wxDefaultPosition, wxSize(700, 400)); // 创建对话框

        // 创建一个wxListCtrl来展示作者信息
        wxListCtrl* authorListCtrl = new wxListCtrl(dialog, wxID_ANY, wxDefaultPosition, wxSize(600, 300), wxLC_REPORT);
        authorListCtrl->InsertColumn(0, wxT("Author"), wxLIST_FORMAT_LEFT, 200);
        authorListCtrl->InsertColumn(1, wxT("Co-Authors"), wxLIST_FORMAT_LEFT, 250);
        authorListCtrl->InsertColumn(2, wxT("Common Works"), wxLIST_FORMAT_LEFT, 250);

        // 创建一个进度条
        wxGauge* progressBar = new wxGauge(dialog, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
        progressBar->SetValue(0); // 初始化进度条的值为0

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(authorListCtrl, 1, wxEXPAND | wxALL, 5);
        sizer->Add(progressBar, 0, wxEXPAND | wxALL, 5);

        dialog->SetSizer(sizer); // 使用sizer来管理对话框的布局

        dialog->Show();
        dialog->Centre();

        // 启动填充作者列表的线程
        std::thread populateThread(&MyFrame::PopulateAuthors, this, authorListCtrl, progressBar);
        populateThread.detach(); // 从主线程分离，独立运行
    }
}

void MyFrame::PopulateAuthors(wxListCtrl* authorListCtrl, wxGauge* progressBar)
{
    int numAuthors = authors.size();
    int processedAuthors = 0;
    for (const auto& author : authors) 
    {
        processedAuthors++;// 更新认为已处理的数
        vector<string>works;
        auto range = author_article.equal_range(author);
        if (range.first != author_article.end())
        {            
            for (auto it = range.first; it != range.second; ++it)// 处理多篇文章的情况
                works.push_back(it->second);
            int size = works.size();
            unordered_set<string>coauthors = find_coauthors(author);
            for (auto it = coauthors.begin(); it != coauthors.end(); ++it)
            {
                auto range = author_article.equal_range(*it);
                vector<string>coworkers;              
                for (auto it2 = range.first; it2 != range.second; ++it2)// 处理多篇文章的情况
                    coworkers.push_back(it2->second);
                wxString result;
                for (int i = 0; i < size; i++)
                    for (int j = 0; j < coworkers.size(); j++)
                        if (works[i] == coworkers[j])
                        {
                            result = result + works[i] + "\n";
                            coworkers.erase(coworkers.begin() + j);
                            break;
                        }
                long itemIndex;
                if (it == coauthors.begin()++)
                    itemIndex = authorListCtrl->InsertItem(authorListCtrl->GetItemCount(), author);
                else
                    itemIndex = authorListCtrl->InsertItem(authorListCtrl->GetItemCount(), "");

                authorListCtrl->SetItem(itemIndex, 1, *it);
                authorListCtrl->SetItem(itemIndex, 2, result);
            }
        }              
        int progress = static_cast<int>((static_cast<float>(processedAuthors) / numAuthors) * 100);// 设置进度条的值为当前进度
        progressBar->SetValue(progress);        
        if (processedAuthors % 10 == 0) // 为了性能考虑，不是每次迭代都更新 
            wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_MY_UPDATE_PROGRESS, progress));
    }
}

//响应进度条更新
void MyFrame::OnUpdateProgress(wxThreadEvent& event)
{
    // 获取事件中的进度值
    int progress = event.GetInt(); 
    // 确保进度条已经被创建并设置进度条的值
    if (progressBar)
    { 
        progressBar->SetValue(progress);
    }
}