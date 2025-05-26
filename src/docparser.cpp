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
#include <magic.h>

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

/**
 * @brief Check if a file is a text file using libmagic MIME type detection
 * @param filename The path to the file to check
 * @return true if the file is detected as text, false otherwise
 */
static bool isTextFileByMimeType(const std::string &filename)
{
    magic_t magic_cookie = magic_open(MAGIC_MIME_TYPE);
    if (magic_cookie == nullptr) {
        std::cerr << "ERROR: [isTextFileByMimeType] Failed to initialize libmagic" << std::endl;
        return false;
    }

    if (magic_load(magic_cookie, nullptr) != 0) {
        std::cerr << "ERROR: [isTextFileByMimeType] Failed to load magic database: "
                  << magic_error(magic_cookie) << std::endl;
        magic_close(magic_cookie);
        return false;
    }

    const char *mime_type = magic_file(magic_cookie, filename.c_str());
    if (mime_type == nullptr) {
        std::cerr << "ERROR: [isTextFileByMimeType] Failed to detect MIME type for "
                  << filename << ": " << magic_error(magic_cookie) << std::endl;
        magic_close(magic_cookie);
        return false;
    }

    std::string mimeStr(mime_type);
    magic_close(magic_cookie);

    std::cout << "INFO: [isTextFileByMimeType] Detected MIME type: " << mimeStr
              << " for file: " << filename << std::endl;

    // Check if MIME type starts with "text/"
    bool isText = mimeStr.substr(0, 5) == "text/";

    // Also consider some application types that are actually text
    if (!isText) {
        static const std::unordered_set<std::string> textApplicationTypes = {
            "application/json",
            "application/xml",
            "application/javascript",
            "application/x-sh",
            "application/x-shellscript",
            "application/x-perl",
            "application/x-python",
            "application/x-ruby",
            "application/x-awk",
            "application/x-desktop",
            "application/x-yaml"
        };

        isText = textApplicationTypes.count(mimeStr) > 0;
    }

    return isText;
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

// Caching the mapping of extensions to creation functions
static const std::unordered_map<std::string, FileCreator> createExtensionMap()
{
    return {
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
}

static const std::unordered_map<std::string, std::string> createSimilarExtensionMap()
{
    return {
        { "doc", "docx" },
        { "docx", "doc" },
        { "xls", "xlsx" },
        { "xlsx", "xls" },
        { "ppt", "pptx" },
        { "pptx", "ppt" }
    };
}

static std::string doConvertFile(const std::string &filename, std::string suffix)
{
    static const std::unordered_map<std::string, FileCreator> extensionMap = createExtensionMap();
    static const std::unordered_map<std::string, std::string> similarExtensionMap = createSimilarExtensionMap();

    // Convert suffix to lowercase
    std::transform(suffix.begin(), suffix.end(), suffix.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    std::unique_ptr<fileext::FileExtension> document;

    try {
        // First check if it is a text file
        if (isTextSuffix(suffix)) {
            document = createTxt(filename, suffix);
        } else {
            // Find the corresponding creation function
            auto it = extensionMap.find(suffix);
            if (it != extensionMap.end()) {
                document = it->second(filename, suffix);
            } else {
                // Extension not found in map, check if it's a text file by content
                std::cout << "INFO: [doConvertFile] Unknown file extension '" << suffix
                          << "', checking file content for text type: " << filename << std::endl;

                if (isTextFileByMimeType(filename)) {
                    std::cout << "INFO: [doConvertFile] File detected as text by MIME type analysis: "
                              << filename << std::endl;
                    document = createTxt(filename, suffix);
                } else {
                    throw std::logic_error("Unsupported file extension: " + filename);
                }
            }
        }

        document->convert();
        // Use move semantics to avoid copying
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
    static const std::unordered_map<std::string, FileCreator> extensionMap = createExtensionMap();
    static const std::unordered_map<std::string, std::string> similarExtensionMap = createSimilarExtensionMap();

    auto it = similarExtensionMap.find(suffix);
    if (it != similarExtensionMap.end()) {
        return doConvertFile(filename, it->second);
    }

    return {};
}
