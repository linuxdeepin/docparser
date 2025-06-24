/**
 * @brief   Interface for file extensions
 * @package fileext
 * @file    fileext.hpp
 * @author  dmryutov (dmryutov@gmail.com)
 * @version 1.1.1
 * @date    12.07.2016 -- 10.02.2018
 */
#pragma once

// Uncomment this line to enable downloading images from URL (requires `cUrl` library)
// #define DOWNLOAD_IMAGES

#include <string>
#include <vector>
#include <pugixml.hpp>


/**
 * @namespace fileext
 * @brief
 *     Interface for file extensions
 */
namespace fileext {

/**
 * @class FileExtension
 * @brief
 *     Base class for file extensions converters
 */
class FileExtension {
public:
	/**
	 * @param[in] fileName
	 *     File name
	 * @since 1.0
	 */
	FileExtension(const std::string& fileName);

	/** Destructor */
	virtual ~FileExtension() = default;

	/**
	 * @brief
	 *     Convert file to HTML-tree
	 * @param[in] addStyle
	 *     Should read and add styles to HTML-tree
	 * @param[in] extractImages
	 *     True if should extract images
	 * @param[in] mergingMode
	 *     Colspan/rowspan processing mode
	 * @since 1.0
	 */
    virtual int convert(bool addStyle = true, bool extractImages = false, char mergingMode = 0) = 0;

	/** Result HTML tree */
	pugi::xml_document m_htmlTree;

    std::string m_text = "";

	/**
	 * @brief Set truncation limit for content processing
	 * @param[in] maxBytes Maximum bytes to process (0 = no limit)
	 * @since 1.1.2
	 */
	void setTruncationLimit(size_t maxBytes);

	/**
	 * @brief Check if content was truncated during processing
	 * @return true if content was truncated, false otherwise
	 * @since 1.1.2
	 */
	bool isTruncated() const { return m_truncated; }

	/**
	 * @brief Apply final truncation to content (public interface)
	 * @param[in] content Content to truncate
	 * @param[in] maxLength Maximum length allowed
	 * @return Truncated content
	 * @since 1.1.2
	 */
	std::string applyFinalTruncation(const std::string& content, size_t maxLength);

	/**
	 * @brief Mark content as truncated (for external truncation)
	 * @since 1.1.2
	 */
	void markAsTruncated() { m_truncated = true; }

protected:
//    int m_maxLen = 0;
	/** Name of processing file */
	const std::string m_fileName;
	/** Should read and add styles to HTML-tree */
	bool m_addStyle = true;
	/**
	 * Colspan/rowspan processing mode
	 * Value | Description
	 * :---: | -----------
	 *   0   | Cells are merged
	 *   1   | Delete merging, fill cells with duplicate values
	 *   2   | Delete merging, fill cells with empty values
	 */
	char m_mergingMode = 0;
	/** True if should extract images */
	bool m_extractImages = false;
	/** List of images (binary data and extension) */
	std::vector<std::pair<std::string, std::string>> m_imageList;

	/** Truncation control members */
	size_t m_maxBytes = 0;           // 0 means no limit
	bool m_truncationEnabled = false; // Truncation switch
	bool m_truncated = false;        // Truncation status flag

	/**
	 * @brief Safely append text with truncation control
	 * @param[in] text Text to append
	 * @return true if text was appended, false if truncation occurred
	 * @since 1.1.2
	 */
	bool safeAppendText(const std::string& text);

	/**
	 * @brief Check if processing should stop due to truncation
	 * @return true if processing should stop
	 * @since 1.1.2
	 */
	bool shouldStopProcessing() const;

	/**
	 * @brief Truncate text at reasonable boundary (sentence, word, etc.)
	 * @param[in] text Text to truncate
	 * @param[in] maxLength Maximum length allowed
	 * @return Truncated text
	 * @since 1.1.2
	 */
	std::string truncateAtBoundary(const std::string& text, size_t maxLength) const;
};

}  // End namespace
