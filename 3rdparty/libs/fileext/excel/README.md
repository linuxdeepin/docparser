# Excel 解析模块 / Excel Parsing Module

## 概述 / Overview

本模块负责从 Excel 文件（`.xlsx` 和 `.xls`）中提取纯文本内容，供全文检索使用。

This module extracts plain text content from Excel files (`.xlsx` and `.xls`) for full-text search purposes.

## 文件结构 / File Structure

```
excel/
├── excel.cpp / excel.hpp           # 入口类，根据扩展名分发解析
├── excel_xlsxio.cpp / .hpp         # XLSX 解析（基于 xlsxio SAX 流式）
├── excel_libxls.cpp / .hpp          # XLS 解析（基于 libxls）
├── xlsxio/                          # xlsxio 库源码
│   ├── xlsxio_read.c                #   SAX 流式读取实现
│   ├── xlsxio_read_sharedstrings.c  #   共享字符串表处理
│   └── xlsxio_read.h / *.h         #   头文件
└── libxls/                          # libxls 库源码
    ├── xls.c / ole.c / xlstool.c    #   OLE + BIFF 解析实现
    ├── endian.c / locale.c          #   字节序与编码处理
    └── include/                     #   头文件
        ├── xls.h
        └── libxls/
```

## 架构 / Architecture

```
Excel::convert()
    │
    ├── .xlsx ──→ parseXlsxWithXlsxio()
    │               └── xlsxio (SAX 流式解析，expat + minizip)
    │
    └── .xls  ──→ parseXlsWithLibxls()
                    └── libxls (OLE + BIFF 解析，内置 UTF-8 转换)
```

## 外部依赖 / External Dependencies

| 依赖 | 用途 | 许可证 |
|------|------|--------|
| expat | xlsxio 的 XML SAX 解析 | MIT |
| minizip | xlsxio 的 ZIP 解压 | Zlib |
| zlib | minizip 的底层依赖 | Zlib |
| iconv | libxls 的编码转换（系统库） | LGPL |

xlsxio 和 libxls 的源码已直接包含在本目录中，无需额外下载。

The xlsxio and libxls sources are bundled locally; no additional download required.

## 输出格式 / Output Format

每个非空单元格的值后跟换行符 `\n`，所有工作表的内容顺序拼接：

Each non-empty cell value is followed by a newline `\n`; all sheet contents are concatenated in order:

```
A1的值
B1的值
A2的值
...
```

## 第三方库版本 / Bundled Library Versions

- **xlsxio**: 基于 [brechtsanders/xlsxio](https://github.com/brechtsanders/xlsxio) (MIT License)
- **libxls**: 基于 [libxls/libxls](https://github.com/libxls/libxls) v1.6.3 (BSD-2-Clause License)
