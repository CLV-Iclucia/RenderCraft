/**
 * @note this file is discarded, because there is no need to implement a xml parser myself
 *      I will use external dependencies instead.
 */


#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <memory>
#include <cassert>
#include "../render/Scene.h"

#define TAG_OPEN 0
#define TAG_CLOSE 1
#define TAG_SINGLE 2
#define TAG_ERROR 3
#define INVALID_TAG_NAME_NOT_START_WITH_ALPHA_OR_UNDERLINE 0
#define INVALID_TAG_NAME_START_WITH_XML 1
#define VALID_TAG_NAME 2
#define THROW_PARSE_ERROR {std::cerr << "Error: Cannot parse scene description file. Please check the file format." << std::endl;\
                            return nullptr;}

int read_tag(std::fstream& fin, std::string& tag)
{
    char ch;
    bool ignore = false;
    while((ch = fin.get()))
    {
        if(ch != EOF)
        {
            if(isspace(ch)) continue;
            else break;
        }
        else return false;
    }
    tag = ch;
    while((ch = fin.get()))
    {
        if(isspace(ch)) tag += ' ';
        else tag += ch;
        if(ch == '\"')
        {
            ignore = !ignore;
            continue;
        }
        if(ignore) continue;
        if(ch == '>') return true;
    }
    return false;
}

int classify_tag(const std::string& tag)
{
    int n = tag.length();
    if(tag[n - 2] == '/') return TAG_SINGLE;
    else if(tag[1] == '/') return TAG_CLOSE;
    else return TAG_OPEN;
}

int get_tag_name(const std::string& tag, std::string& tag_name, int tag_type)
{
    int start_idx = (tag_type == TAG_CLOSE) ? 2 : 1;
    if(!isalpha(tag[start_idx]) && tag[start_idx] != '_') return INVALID_TAG_NAME_NOT_START_WITH_ALPHA_OR_UNDERLINE;
    if(tag.length() > start_idx + 3)
    {
        if((tag[start_idx] == 'x' || tag[start_idx] == 'X') && (tag[start_idx + 1] == 'm'
                                                                || tag[start_idx + 1] == 'M') && (tag[start_idx + 2] == 'l' || tag[start_idx + 2] == 'L'))
            return INVALID_TAG_NAME_START_WITH_XML;
    }
    int len = 0;
    for(int i = start_idx; i < tag.length(); i++)
    {
        if(!isalpha(tag[i]) && !isdigit(tag[i]) && tag[i] != '_' && tag[i] != '-' && tag[i] != '.') break;
        len++;
    }
    tag_name = tag.substr(start_idx, len);
    return VALID_TAG_NAME;
}

struct DOMNode
{
    std::string tag;
    std::string tag_name;
    std::shared_ptr<DOMNode> ch, nxt;
    DOMNode(std::string _tag, std::string _tag_name) : tag(std::move(_tag)), tag_name(std::move(_tag_name)) {}
    explicit DOMNode(std::string _tag)
    {
        get_tag_name(_tag, tag_name, classify_tag(_tag));
        tag = std::move(_tag);
    }
    void add_child(const std::shared_ptr<DOMNode>& nd)
    {
        assert(!nd->nxt);
        nd->nxt = ch;
        ch = nd;
    }
};

std::stack<std::shared_ptr<DOMNode> > stk;

void dfs(const std::shared_ptr<DOMNode>& o)
{
    for(auto ptr = o->ch; ptr; ptr = ptr->nxt)
    {
        dfs(ptr);

    }
}

Scene* parse_scene(const std::shared_ptr<DOMNode>& rt)
{
    return nullptr;
}

Scene* parse_xml(const std::string &path)
{
    std::fstream fin(path);
    std::string tag;
    std::shared_ptr<DOMNode> rt =  std::make_shared<DOMNode>("", "xml");
    stk.push(rt);
    while(true)
    {
        if(!read_tag(fin, tag)) break;
        int n = tag.length();
        if(tag[0] != '<' || tag[n - 1] != '>') THROW_PARSE_ERROR;
        int type = classify_tag(tag);
        std::string tag_name;
        int error_code = get_tag_name(tag, tag_name, type);
        if(error_code == INVALID_TAG_NAME_NOT_START_WITH_ALPHA_OR_UNDERLINE)
        {
            std::cerr << "Invalid tag name: tag name should start with alpha or underline." << std::endl;
            return nullptr;
        }
        else if(error_code == INVALID_TAG_NAME_START_WITH_XML)
        {
            std::cerr << "Invalid tag name: tag name cannot start with \"xml\"." << std::endl;
            return nullptr;
        }
        std::shared_ptr<DOMNode> nd = std::make_shared<DOMNode>(tag, tag_name);
        if(type == TAG_OPEN || type == TAG_SINGLE)
        {
            if(!stk.empty())
            {
                stk.top()->add_child(nd);
                if(type == TAG_OPEN) stk.push(nd);
            }
        }
        else if(type == TAG_CLOSE)
        {
            if(tag_name == stk.top()->tag_name) stk.pop();
            else
            {
                std::cerr << "Error: Cannot parse scene description file: tag names don't match." << std::endl;
                return nullptr;
            }
        }
    }
    if(stk.size() != 1) THROW_PARSE_ERROR;
    return parse_scene(rt);
}