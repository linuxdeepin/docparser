/**
 * @brief   Interface for file extensions
 * @package fileext
 * @file    fileext.cpp
 * @author  dmryutov (dmryutov@gmail.com)
 * @version 1.1.1
 * @date    12.07.2016 -- 10.02.2018
 */
#include <fstream>

#include "tools.hpp"

#include "fileext.hpp"
#include <iostream>
#include <algorithm>

namespace fileext {

/** Config script file path */
const std::string LIB_PATH = tools::PROGRAM_PATH + "/files/libs";
const std::string SCRIPT_FILE = LIB_PATH + "/xpathconfig.min.js";

// public:
FileExtension::FileExtension(const std::string& fileName)
	: m_fileName(fileName) {}

void FileExtension::setTruncationLimit(size_t maxBytes)
{
	m_maxBytes = maxBytes;
	m_truncationEnabled = true; // Always enable when limit is set, even if 0
	m_truncated = false;
}

bool FileExtension::safeAppendText(const std::string& text)
{
	if (!m_truncationEnabled) {
		m_text += text;
		return true;
	}

	// Special case: if maxBytes is 0, don't add anything
	if (m_maxBytes == 0) {
		m_truncated = true;
		return false;
	}

	// Check if adding this text would exceed the limit
	size_t currentSize = m_text.size();
	size_t newTextSize = text.size();
	
	if (currentSize >= m_maxBytes) {
		// Already at or over limit, don't add anything
		m_truncated = true;
		return false;
	}
	
	if (currentSize + newTextSize <= m_maxBytes) {
		// Safe to add all text
		m_text += text;
		return true;
	}
	
	// Need to truncate the new text
	size_t remainingBytes = m_maxBytes - currentSize;
	std::string truncatedText = truncateAtBoundary(text, remainingBytes);
	
	m_text += truncatedText;
	m_truncated = true;
	
	return false; // Indicate truncation occurred
}

bool FileExtension::shouldStopProcessing() const
{
	return m_truncationEnabled && m_text.size() >= m_maxBytes;
}

std::string FileExtension::truncateAtBoundary(const std::string& text, size_t maxLength) const
{
	if (text.size() <= maxLength) {
		return text;
	}
	
	if (maxLength == 0) {
		return "";
	}
	
	// Try to find a good boundary to truncate at
	std::string truncated = text.substr(0, maxLength);
	
	// Look for sentence boundaries (. ! ?) within the last 50 characters
	size_t searchStart = (maxLength > 50) ? maxLength - 50 : 0;
	for (size_t i = maxLength - 1; i > searchStart; --i) {
		char c = text[i];
		if (c == '.' || c == '!' || c == '?' || c == '\n') {
			return text.substr(0, i + 1);
		}
	}
	
	// Look for word boundaries (spaces) within the last 20 characters
	searchStart = (maxLength > 20) ? maxLength - 20 : 0;
	for (size_t i = maxLength - 1; i > searchStart; --i) {
		char c = text[i];
		if (c == ' ' || c == '\t' || c == '\n') {
			return text.substr(0, i);
		}
	}
	
	// If no good boundary found, just truncate at the limit
	return truncated;
}

std::string FileExtension::applyFinalTruncation(const std::string& content, size_t maxLength)
{
	return truncateAtBoundary(content, maxLength);
}

}  // End namespace
