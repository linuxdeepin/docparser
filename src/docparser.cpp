/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liuzhangjian<liuzhangjian@uniontech.com>
 *
 * Maintainer: liuzhangjian<liuzhangjian@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "docparser.h"

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

std::string DocParser::convertFile(const std::string &filename)
{
    std::string content = "";
    std::string suffix = filename.substr(filename.find_last_of('.') + 1);
    if (suffix.empty())
        return content;

    std::unique_ptr<fileext::FileExtension> document;
    try {
        // 比较后缀名，不区分大小写
        if (!strcasecmp(suffix.c_str(), "docx")) {
            document.reset(new docx::Docx(filename));
        } else if (!strcasecmp(suffix.c_str(), "pptx") || !strcasecmp(suffix.c_str(), "ppsx")) {
            document.reset(new pptx::Pptx(filename));
        } else if (!strcasecmp(suffix.c_str(), "txt") || !strcasecmp(suffix.c_str(), "text") ||
                   !strcasecmp(suffix.c_str(), "md") || !strcasecmp(suffix.c_str(), "markdown")) {
            document.reset(new txt::Txt(filename));
        } else if (!strcasecmp(suffix.c_str(), "doc") || !strcasecmp(suffix.c_str(), "dot") ||
                   !strcasecmp(suffix.c_str(), "wps")) {
            document.reset(new doc::Doc(filename));
        } else if (!strcasecmp(suffix.c_str(), "rtf")) {
            document.reset(new rtf::Rtf(filename));
        } else if (!strcasecmp(suffix.c_str(), "odg") || !strcasecmp(suffix.c_str(), "odt") ||
                   !strcasecmp(suffix.c_str(), "ods") || !strcasecmp(suffix.c_str(), "odp")) {
            document.reset(new odf::Odf(filename));
        } else if (!strcasecmp(suffix.c_str(), "xls") || !strcasecmp(suffix.c_str(), "xlsx")) {
            document.reset(new excel::Excel(filename, suffix));
        } else if (!strcasecmp(suffix.c_str(), "xlsb")) {
            document.reset(new xlsb::Xlsb(filename));
        } else if (!strcasecmp(suffix.c_str(), "ppt") || !strcasecmp(suffix.c_str(), "pps") ||
                   !strcasecmp(suffix.c_str(), "dps")) {
            document.reset(new ppt::Ppt(filename));
        } else if (!strcasecmp(suffix.c_str(), "pdf")) {
            document.reset(new pdf::Pdf(filename));
        } else {
            std::cout << "Unsupported file extension: " << filename << std::endl;
            throw "";
        }

        document->convert();
        content = document->m_text;
    } catch (...) {
        std::cerr << "Parse failed: " << filename << std::endl;
    }

    document.reset();
    return content;
}
