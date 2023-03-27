// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "docparser/docparser.h"

#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "usage: doc_parser filename" << std::endl;
        return 1;
    }

    std::string file(argv[1]);
    auto documents = DocParser::convertFile(file);

    std::cout << documents.c_str() << std::endl;

    return 0;
}
