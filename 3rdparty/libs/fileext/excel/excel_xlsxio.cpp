#include "excel_xlsxio.hpp"

#include <xlsxio_read.h>

#include <cstring>
#include <string>
#include <vector>

namespace excel {

static int sheetNameCallback(const char* name, void* callbackdata) {
    auto* names = static_cast<std::vector<std::string>*>(callbackdata);
    if (name) {
        names->push_back(name);
    }
    return 0;
}

int parseXlsxWithXlsxio(const std::string& filename, std::string& output) {
    xlsxioreader handle = xlsxioread_open(filename.c_str());
    if (!handle) {
        return -1;
    }

    std::vector<std::string> sheetNames;
    xlsxioread_list_sheets(handle, sheetNameCallback, &sheetNames);

    for (const auto& sheetName : sheetNames) {
        xlsxioreadersheet sheet = xlsxioread_sheet_open(
            handle, sheetName.c_str(), XLSXIOREAD_SKIP_EMPTY_ROWS);
        if (!sheet) {
            continue;
        }

        while (xlsxioread_sheet_next_row(sheet)) {
            char* value = nullptr;
            while ((value = xlsxioread_sheet_next_cell(sheet)) != nullptr) {
                if (value[0] != '\0') {
                    output += value;
                    output += '\n';
                }
                xlsxioread_free(value);
            }
        }

        xlsxioread_sheet_close(sheet);
    }

    xlsxioread_close(handle);
    return 0;
}

}  // namespace excel
