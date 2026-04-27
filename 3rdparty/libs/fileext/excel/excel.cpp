/**
 * @brief     Excel files (xls/xlsx) text extractor
 * @package   excel
 * @file      excel.cpp
 * @author    dmryutov (dmryutov@gmail.com)
 * @date      02.12.2016 -- 28.01.2018
 */
#include <fstream>
#include <string.h>

#include "tools.hpp"

#include "excel_xlsxio.hpp"
#include "excel_libxls.hpp"

#include "excel.hpp"


namespace excel {

Excel::Excel(const std::string& fileName, const std::string& extension)
	: FileExtension(fileName), m_extension(extension) {}

int Excel::convert(bool addStyle, bool extractImages, char mergingMode) {
	int result = -1;

	if (!strcasecmp(m_extension.c_str(), "xlsx")) {
		result = parseXlsxWithXlsxio(m_fileName, m_text);
	} else {
		result = parseXlsWithLibxls(m_fileName, m_text);
	}

	if (result != 0) {
		m_text.clear();
	}

	if (m_truncationEnabled && m_text.size() > m_maxBytes) {
		m_text = truncateAtBoundary(m_text, m_maxBytes);
		m_truncated = true;
	}

	return result;
}

}
