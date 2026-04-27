/**
 * @brief     Excel files (xls/xlsx) text extractor
 * @package   excel
 * @file      excel.hpp
 * @author    dmryutov (dmryutov@gmail.com)
 * @version   2.0.0
 * @date      02.12.2016 -- 27.04.2026
 */
#pragma once

#include <string>

#include "fileext/fileext.hpp"


namespace excel {

class Excel: public fileext::FileExtension {
public:
	Excel(const std::string& fileName, const std::string& extension);

	virtual ~Excel() = default;

	int convert(bool addStyle = true, bool extractImages = false, char mergingMode = 0) override;

private:
	const std::string m_extension;
};

}  // End namespace
