#include "excel_libxls.hpp"

#include <xls.h>

#include <cstring>

namespace excel {

int parseXlsWithLibxls(const std::string& filename, std::string& output) {
    xls::xls_error_t err;
    xls::xlsWorkBook* wb = xls::xls_open_file(filename.c_str(), "UTF-8", &err);
    if (!wb) {
        return -1;
    }

    err = xls::xls_parseWorkBook(wb);
    if (err != xls::LIBXLS_OK) {
        xls::xls_close_WB(wb);
        return -1;
    }

    for (int i = 0; i < wb->sheets.count; i++) {
        xls::xlsWorkSheet* ws = xls::xls_getWorkSheet(wb, i);
        if (!ws) {
            continue;
        }

        err = xls::xls_parseWorkSheet(ws);
        if (err != xls::LIBXLS_OK) {
            xls::xls_close_WS(ws);
            continue;
        }

        for (int row = 0; row <= ws->rows.lastrow; row++) {
            for (int col = 0; col <= ws->rows.lastcol; col++) {
                xls::xlsCell* cell = xls::xls_cell(ws, row, col);
                if (!cell || cell->isHidden) {
                    continue;
                }
                if (cell->str && cell->str[0] != '\0') {
                    output += cell->str;
                    output += '\n';
                }
            }
        }

        xls::xls_close_WS(ws);
    }

    xls::xls_close_WB(wb);
    return 0;
}

}  // namespace excel
