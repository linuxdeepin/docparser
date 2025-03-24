// SPDX-FileCopyrightText: 2021 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "docparser.h"
#include "ofd/ofd.h"

#include "fileext/doc/doc.hpp"
#include "fileext/docx/docx.hpp"
#include "fileext/excel/excel.hpp"
#include "fileext/odf/odf.hpp"
#include "fileext/pdf/pdf.hpp"
#include "fileext/ppt/ppt.hpp"
#include "fileext/pptx/pptx.hpp"
#include "fileext/rtf/rtf.hpp"
#include "fileext/txt/txt.hpp"
#include "fileext/xlsb/xlsb.h"

#include <memory>
#include <iostream>
#include <cstring>
#include <unordered_set>
#include <unordered_map>
#include <string_view>
#include <algorithm>

static bool isTextSuffix(std::string_view suffix)
{
    static const std::unordered_set<std::string_view> validSuffixes = {
        "txt", "text", "md", "markdown", "sh", "html", "htm",
        "xml", "xhtml", "dhtml", "shtm", "shtml", "json",
        "css", "yaml", "ini", "bat", "js", "sql", "uof"
    };

    std::string lowercaseSuffix(suffix);
    std::transform(lowercaseSuffix.begin(), lowercaseSuffix.end(), lowercaseSuffix.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return validSuffixes.count(lowercaseSuffix) > 0;
}

// 预处理后缀映射，避免多次strcasecmp比较
using FileCreator = std::unique_ptr<fileext::FileExtension> (*)(const std::string &, const std::string &);

static std::unique_ptr<fileext::FileExtension> createDocx(const std::string &filename, const std::string &)
{
    return std::make_unique<docx::Docx>(filename);
}

static std::unique_ptr<fileext::FileExtension> createPptx(const std::string &filename, const std::string &)
{
    return std::make_unique<pptx::Pptx>(filename);
}

static std::unique_ptr<fileext::FileExtension> createTxt(const std::string &filename, const std::string &)
{
    return std::make_unique<txt::Txt>(filename);
}

static std::unique_ptr<fileext::FileExtension> createDoc(const std::string &filename, const std::string &)
{
    return std::make_unique<doc::Doc>(filename);
}

static std::unique_ptr<fileext::FileExtension> createRtf(const std::string &filename, const std::string &)
{
    return std::make_unique<rtf::Rtf>(filename);
}

static std::unique_ptr<fileext::FileExtension> createOdf(const std::string &filename, const std::string &)
{
    return std::make_unique<odf::Odf>(filename);
}

static std::unique_ptr<fileext::FileExtension> createExcel(const std::string &filename, const std::string &suffix)
{
    return std::make_unique<excel::Excel>(filename, suffix);
}

static std::unique_ptr<fileext::FileExtension> createXlsb(const std::string &filename, const std::string &)
{
    return std::make_unique<xlsb::Xlsb>(filename);
}

static std::unique_ptr<fileext::FileExtension> createPpt(const std::string &filename, const std::string &)
{
    return std::make_unique<ppt::Ppt>(filename);
}

static std::unique_ptr<fileext::FileExtension> createPdf(const std::string &filename, const std::string &)
{
    return std::make_unique<pdf::Pdf>(filename);
}

static std::unique_ptr<fileext::FileExtension> createOfd(const std::string &filename, const std::string &)
{
    return std::make_unique<ofd::Ofd>(filename);
}

// 缓存后缀到创建函数的映射
static const std::unordered_map<std::string, FileCreator> extensionMap = {
    { "docx", createDocx },
    { "pptx", createPptx },
    { "ppsx", createPptx },
    { "doc", createDoc },
    { "dot", createDoc },
    { "wps", createDoc },
    { "rtf", createRtf },
    { "odg", createOdf },
    { "odt", createOdf },
    { "ods", createOdf },
    { "odp", createOdf },
    { "xls", createExcel },
    { "xlsx", createExcel },
    { "xlsb", createXlsb },
    { "ppt", createPpt },
    { "pps", createPpt },
    { "dps", createPpt },
    { "pot", createPpt },
    { "pdf", createPdf },
    { "ofd", createOfd }
};

// 缓存相似后缀的映射关系
static const std::unordered_map<std::string, std::string> similarExtensionMap = {
    { "doc", "docx" },
    { "docx", "doc" },
    { "xls", "xlsx" },
    { "xlsx", "xls" },
    { "ppt", "pptx" },
    { "pptx", "ppt" }
};

static std::string doConvertFile(const std::string &filename, std::string suffix)
{
    // 转换后缀为小写
    std::transform(suffix.begin(), suffix.end(), suffix.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    std::unique_ptr<fileext::FileExtension> document;

    try {
        // 先检查是否是文本文件
        if (isTextSuffix(suffix)) {
            document = createTxt(filename, suffix);
        } else {
            // 查找对应的创建函数
            auto it = extensionMap.find(suffix);
            if (it != extensionMap.end()) {
                document = it->second(filename, suffix);
            } else {
                throw std::logic_error("Unsupported file extension: " + filename);
            }
        }

        document->convert();
        // 使用移动语义避免复制
        return std::move(document->m_text);
    } catch (const std::logic_error &error) {
        std::cout << error.what() << std::endl;
    } catch (...) {
        std::cerr << "Parse failed: " << filename << std::endl;
    }

    return {};
}

std::string DocParser::convertFile(const std::string &filename)
{
    // 更高效地查找最后一个点的位置
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == filename.length() - 1)
        return {};

    std::string suffix = filename.substr(dotPos + 1);
    std::transform(suffix.begin(), suffix.end(), suffix.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    // 尝试使用原始后缀解析
    std::string content = doConvertFile(filename, suffix);
    if (!content.empty())
        return content;

    // 尝试相似后缀
    auto it = similarExtensionMap.find(suffix);
    if (it != similarExtensionMap.end()) {
        return doConvertFile(filename, it->second);
    }

    return {};
}
