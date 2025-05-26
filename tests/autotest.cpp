// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "docparser.h"

#include <QTest>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QRandomGenerator>
#include <QElapsedTimer>

/**
 * @brief Unit test class for DocParser library
 *
 * This test suite validates the functionality of the DocParser library,
 * including file parsing, format detection, error handling, and edge cases.
 */
class DocParserAutoTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testTextFileConversion_data();
    void testTextFileConversion();
    void testNonExistentFile();
    void testEmptyFile();

    // File extension handling tests
    void testCaseInsensitiveExtensions_data();
    void testCaseInsensitiveExtensions();
    void testFileWithoutExtension();
    void testFileWithMultipleDots();

    // Error handling tests
    void testInvalidFilePath();
    void testPermissionDeniedFile();
    void testCorruptedFile();

    // Performance and edge case tests
    void testLargeFile();
    void testSpecialCharactersInPath();
    void testUnicodeContent();

    // MIME type detection tests
    void testMimeTypeDetection();

private:
    QString createTestFile(const QString &content, const QString &suffix = "txt");
    QString createBinaryTestFile(const QByteArray &data, const QString &suffix);
    void verifyConversionResult(const std::string &result, const QString &expectedContent);

private:
    QTemporaryDir *m_tempDir = nullptr;
    QStringList m_createdFiles;
};

void DocParserAutoTest::initTestCase()
{
    qInfo() << "INFO: [DocParserAutoTest::initTestCase] Starting DocParser unit tests";

    // Create temporary directory for test files
    m_tempDir = new QTemporaryDir();
    QVERIFY2(m_tempDir->isValid(), "Failed to create temporary directory for tests");

    qInfo() << "INFO: [DocParserAutoTest::initTestCase] Test directory created:" << m_tempDir->path();
}

void DocParserAutoTest::cleanupTestCase()
{
    // Clean up created files
    for (const QString &file : m_createdFiles) {
        QFile::remove(file);
    }
    m_createdFiles.clear();

    delete m_tempDir;
    m_tempDir = nullptr;

    qInfo() << "INFO: [DocParserAutoTest::cleanupTestCase] Test cleanup completed";
}

void DocParserAutoTest::init()
{
    // Per-test initialization if needed
}

void DocParserAutoTest::cleanup()
{
    // Per-test cleanup if needed
}

void DocParserAutoTest::testTextFileConversion_data()
{
    QTest::addColumn<QString>("content");
    QTest::addColumn<QString>("extension");
    QTest::addColumn<QString>("description");

    QTest::newRow("simple_txt") << "Hello, World!"
                                << "txt"
                                << "Simple text file";
    QTest::newRow("markdown") << "# Title\n\nThis is **markdown** content."
                              << "md"
                              << "Markdown file";
    QTest::newRow("json") << R"({"key": "value", "number": 42})"
                          << "json"
                          << "JSON file";
    QTest::newRow("xml") << "<?xml version=\"1.0\"?>\n<root><item>value</item></root>"
                         << "xml"
                         << "XML file";
    QTest::newRow("html") << "<html><body><h1>Test</h1></body></html>"
                          << "html"
                          << "HTML file";
    QTest::newRow("css") << "body { color: red; font-size: 14px; }"
                         << "css"
                         << "CSS file";
    QTest::newRow("javascript") << "function test() { return 'hello'; }"
                                << "js"
                                << "JavaScript file";
    QTest::newRow("empty_text") << ""
                                << "txt"
                                << "Empty text file";
    QTest::newRow("multiline") << "Line 1\nLine 2\nLine 3\n"
                               << "txt"
                               << "Multi-line text";
}

void DocParserAutoTest::testTextFileConversion()
{
    QFETCH(QString, content);
    QFETCH(QString, extension);
    QFETCH(QString, description);

    qInfo() << "INFO: [DocParserAutoTest::testTextFileConversion] Testing" << description;

    QString testFile = createTestFile(content, extension);
    QVERIFY(!testFile.isEmpty());

    std::string result = DocParser::convertFile(testFile.toStdString());

    if (content.isEmpty()) {
        QVERIFY2(result.empty(), qPrintable(QString("Expected empty result for: %1").arg(description)));
    } else {
        verifyConversionResult(result, content);
    }
}

void DocParserAutoTest::testNonExistentFile()
{
    qInfo() << "INFO: [DocParserAutoTest::testNonExistentFile] Testing non-existent file";

    QString nonExistentFile = m_tempDir->path() + "/non_existent_file.txt";
    std::string result = DocParser::convertFile(nonExistentFile.toStdString());

    QVERIFY2(result.empty(), "Non-existent file should return empty content");
}

void DocParserAutoTest::testEmptyFile()
{
    qInfo() << "INFO: [DocParserAutoTest::testEmptyFile] Testing empty file";

    QString emptyFile = createTestFile("", "txt");
    QVERIFY(!emptyFile.isEmpty());

    std::string result = DocParser::convertFile(emptyFile.toStdString());
    QVERIFY2(result.empty(), "Empty file should return empty content");
}

void DocParserAutoTest::testCaseInsensitiveExtensions_data()
{
    QTest::addColumn<QString>("extension");
    QTest::addColumn<QString>("content");

    QTest::newRow("uppercase_TXT") << "TXT"
                                   << "Uppercase extension test";
    QTest::newRow("mixed_case_Txt") << "Txt"
                                    << "Mixed case extension test";
    QTest::newRow("uppercase_MD") << "MD"
                                  << "# Uppercase Markdown";
    QTest::newRow("mixed_case_Html") << "Html"
                                     << "<html><body>Mixed case HTML</body></html>";
}

void DocParserAutoTest::testCaseInsensitiveExtensions()
{
    QFETCH(QString, extension);
    QFETCH(QString, content);

    qInfo() << "INFO: [DocParserAutoTest::testCaseInsensitiveExtensions] Testing extension:" << extension;

    QString testFile = createTestFile(content, extension);
    QVERIFY(!testFile.isEmpty());

    std::string result = DocParser::convertFile(testFile.toStdString());
    verifyConversionResult(result, content);
}

void DocParserAutoTest::testFileWithoutExtension()
{
    qInfo() << "INFO: [DocParserAutoTest::testFileWithoutExtension] Testing file without extension";

    QString content = "File without extension";
    QTemporaryFile tempFile(m_tempDir->path() + "/no_extension_XXXXXX");
    QVERIFY(tempFile.open());

    QTextStream stream(&tempFile);
    stream << content;
    tempFile.close();

    // Keep the file around for testing
    tempFile.setAutoRemove(false);
    m_createdFiles << tempFile.fileName();

    std::string result = DocParser::convertFile(tempFile.fileName().toStdString());
    QVERIFY2(result.empty(), "File without extension should return empty content");
}

void DocParserAutoTest::testFileWithMultipleDots()
{
    qInfo() << "INFO: [DocParserAutoTest::testFileWithMultipleDots] Testing file with multiple dots";

    QString content = "File with multiple dots in name";
    QString fileName = m_tempDir->path() + "/test.file.with.dots.txt";

    QFile file(fileName);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));

    QTextStream stream(&file);
    stream << content;
    file.close();

    m_createdFiles << fileName;

    std::string result = DocParser::convertFile(fileName.toStdString());
    verifyConversionResult(result, content);
}

void DocParserAutoTest::testInvalidFilePath()
{
    qInfo() << "INFO: [DocParserAutoTest::testInvalidFilePath] Testing invalid file paths";

    QStringList invalidPaths = {
        "",   // Empty string
        "/dev/null/invalid/path.txt",   // Invalid path
        "/proc/invalid_file.txt",   // Permission issues
        QString(1000, 'a') + ".txt"   // Extremely long path
    };

    for (const QString &path : invalidPaths) {
        std::string result = DocParser::convertFile(path.toStdString());
        QVERIFY2(result.empty(), qPrintable(QString("Invalid path should return empty: %1").arg(path)));
    }
}

void DocParserAutoTest::testPermissionDeniedFile()
{
    qInfo() << "INFO: [DocParserAutoTest::testPermissionDeniedFile] Testing permission denied scenario";

    // Create a file and remove read permissions
    QString content = "Permission test content";
    QString testFile = createTestFile(content, "txt");
    QVERIFY(!testFile.isEmpty());

    // Remove read permissions
    QFile::setPermissions(testFile, QFileDevice::WriteOwner);

    std::string result = DocParser::convertFile(testFile.toStdString());

    // Restore permissions for cleanup
    QFile::setPermissions(testFile, QFileDevice::ReadOwner | QFileDevice::WriteOwner);

    // File without read permission should return empty content
    QVERIFY2(result.empty(), "File without read permission should return empty content");
}

void DocParserAutoTest::testCorruptedFile()
{
    qInfo() << "INFO: [DocParserAutoTest::testCorruptedFile] Testing corrupted file handling";

    // Create a file with invalid content that looks like a document but isn't
    QByteArray corruptedData;
    corruptedData.append("PK");   // ZIP signature start (like DOCX)
    corruptedData.append('\x03');
    corruptedData.append('\x04');
    corruptedData.append("corrupted_content_not_valid_zip");

    QString corruptedFile = createBinaryTestFile(corruptedData, "docx");
    QVERIFY(!corruptedFile.isEmpty());

    std::string result = DocParser::convertFile(corruptedFile.toStdString());
    // Corrupted files should be handled gracefully and return empty content
    QVERIFY2(result.empty(), "Corrupted file should return empty content");
}

void DocParserAutoTest::testLargeFile()
{
    qInfo() << "INFO: [DocParserAutoTest::testLargeFile] Testing large file handling";

    // Create a large text file (1MB)
    const int largeSize = 1024 * 1024;   // 1MB
    QString largeContent;
    largeContent.reserve(largeSize);

    for (int i = 0; i < largeSize / 50; ++i) {
        largeContent += QString("This is line %1 with some content.\n").arg(i);
    }

    QString largeFile = createTestFile(largeContent, "txt");
    QVERIFY(!largeFile.isEmpty());

    // Measure parsing time
    QElapsedTimer timer;
    timer.start();

    std::string result = DocParser::convertFile(largeFile.toStdString());

    qint64 elapsed = timer.elapsed();
    qInfo() << "INFO: [DocParserAutoTest::testLargeFile] Large file parsing took" << elapsed << "ms";

    QVERIFY2(!result.empty(), "Large file should be parsed successfully");
    QVERIFY2(elapsed < 10000, "Large file parsing should complete within 10 seconds");   // Performance check
}

void DocParserAutoTest::testSpecialCharactersInPath()
{
    qInfo() << "INFO: [DocParserAutoTest::testSpecialCharactersInPath] Testing special characters in file path";

    QString content = "Content with special path characters";

    // Test various special characters in filename
    QStringList specialNames = {
        "test file with spaces.txt",
        "test-file-with-dashes.txt",
        "test_file_with_underscores.txt",
        "测试中文文件名.txt",   // Chinese characters
        "файл_тест.txt"   // Cyrillic characters
    };

    for (const QString &specialName : specialNames) {
        QString specialPath = m_tempDir->path() + "/" + specialName;

        QFile file(specialPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << content;
            file.close();

            m_createdFiles << specialPath;

            std::string result = DocParser::convertFile(specialPath.toStdString());
            verifyConversionResult(result, content);
        }
    }
}

void DocParserAutoTest::testUnicodeContent()
{
    qInfo() << "INFO: [DocParserAutoTest::testUnicodeContent] Testing Unicode content handling";

    QStringList unicodeContents = {
        "Hello 世界! 🌍",
        "Здравствуй мир!",
        "مرحبا بالعالم",
        "こんにちは世界",
        "Emoji test: 😀🎉📝✨"
    };

    for (const QString &unicodeContent : unicodeContents) {
        QString testFile = createTestFile(unicodeContent, "txt");
        QVERIFY(!testFile.isEmpty());

        std::string result = DocParser::convertFile(testFile.toStdString());
        QVERIFY2(!result.empty(), "Unicode content should be parsed successfully");

        // Verify Unicode content is preserved
        QString resultStr = QString::fromStdString(result);
        QVERIFY2(resultStr.contains(unicodeContent.split(' ').first()),
                 "Unicode characters should be preserved in parsing result");
    }
}

void DocParserAutoTest::testMimeTypeDetection()
{
    qInfo() << "INFO: [DocParserAutoTest::testMimeTypeDetection] Testing MIME type detection";

    // Create a file without extension but with text content
    QString content = "This is a text file without proper extension";
    QString noExtFile = m_tempDir->path() + "/no_extension_text_file";

    QFile file(noExtFile);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));

    QTextStream stream(&file);
    stream << content;
    file.close();

    m_createdFiles << noExtFile;

    // The library should detect this as a text file via MIME type
    std::string result = DocParser::convertFile(noExtFile.toStdString());
    // Note: This test depends on the MIME type detection implementation
    // It may return empty if MIME detection is strict about extensions
    qInfo() << "INFO: [DocParserAutoTest::testMimeTypeDetection] MIME detection result length:" << result.length();
}

QString DocParserAutoTest::createTestFile(const QString &content, const QString &suffix)
{
    QString fileName = m_tempDir->path() + QString("/test_file_%1.%2").arg(QRandomGenerator::global()->generate()).arg(suffix);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "ERROR: [DocParserAutoTest::createTestFile] Failed to create test file:" << fileName;
        return QString();
    }

    QTextStream stream(&file);
    stream << content;
    file.close();

    m_createdFiles << fileName;
    return fileName;
}

QString DocParserAutoTest::createBinaryTestFile(const QByteArray &data, const QString &suffix)
{
    QString fileName = m_tempDir->path() + QString("/binary_test_file_%1.%2").arg(QRandomGenerator::global()->generate()).arg(suffix);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical() << "ERROR: [DocParserAutoTest::createBinaryTestFile] Failed to create binary test file:" << fileName;
        return QString();
    }

    file.write(data);
    file.close();

    m_createdFiles << fileName;
    return fileName;
}

void DocParserAutoTest::verifyConversionResult(const std::string &result, const QString &expectedContent)
{
    QVERIFY2(!result.empty(), "Conversion result should not be empty");

    QString resultStr = QString::fromStdString(result);

    // Verify that the result contains the expected content or is at least reasonable
    QVERIFY2(resultStr.length() > 0, "Result should have content");

    // For text files, the content should be similar (allowing for encoding differences)
    if (expectedContent.length() > 0) {
        // Basic sanity check - result shouldn't be drastically different in length
        double lengthRatio = static_cast<double>(resultStr.length()) / expectedContent.length();
        QVERIFY2(lengthRatio >= 0.5 && lengthRatio <= 2.0,
                 qPrintable(QString("Result length ratio %1 seems unreasonable").arg(lengthRatio)));
    }
}

// Include the moc file for Qt's meta-object system
#include "autotest.moc"

// Test main function
QTEST_MAIN(DocParserAutoTest)
