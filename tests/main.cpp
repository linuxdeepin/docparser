// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "docparser.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFileInfo>
#include <QDebug>
#include <QTextStream>
#include <QMimeDatabase>

#include <DTextEncoding>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("docparser_test");
    QCoreApplication::setApplicationVersion("1.0");

    // Setup command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Document parser test application");
    parser.addHelpOption();
    parser.addVersionOption();

    // Add file path argument
    parser.addPositionalArgument("file", "The file to parse");

    // Process the command line arguments
    parser.process(app);

    // Get positional arguments
    const QStringList args = parser.positionalArguments();
    int truncCount = 0;

    // Check if a file path was provided
    if (args.isEmpty()) {
        qCritical() << "Error: A file path must be provided as an argument";
        parser.showHelp(1);
        return 1;
    }

    // Get the file path
    QString filePath = args.first();

    // Get the tunc count
    if (args.count() == 2) {
        truncCount = args[1].toInt();
    }

    // Check if the file exists
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        qCritical() << "Error: File" << filePath << "does not exist";
        return 1;
    }

    QMimeDatabase mimeDatabase;
    QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath);
    QString mimeTypeName = mimeType.name();
    QByteArray fromEncoding = "utf-8";
    if (mimeTypeName.startsWith("text/"))
        fromEncoding = Dtk::Core::DTextEncoding::detectFileEncoding(filePath);
    // Print file information
    qInfo() << "Parsing file:" << filePath << "From encoding: " << fromEncoding;

    try {
        // Call the docparser library to parse the file
        std::string content = truncCount > 0 ? DocParser::convertFile(filePath.toStdString(), truncCount)
                                             : DocParser::convertFile(filePath.toStdString());

        // Convert encoding
        if (fromEncoding.toLower() != "utf-8") {
            auto in = QByteArray::fromStdString(content);
            QByteArray out;
            Dtk::Core::DTextEncoding::convertTextEncodingEx(in, out, "utf-8", fromEncoding);
            content = out.toStdString();
        }

        // Check the parsing result
        if (content.empty()) {
            qWarning() << "Warning: No content was extracted from the file";
            return 0;
        }

        // Print the parsed content
        QTextStream out(stdout);
        out << "\n=========== File Content ===========\n\n";
        out << QString::fromStdString(content) << "\n";
        out << "\n=================================\n";

        // Print statistics
        qInfo() << "Parsing successful, extracted" << content.length() << "characters";

    } catch (const std::exception &e) {
        qCritical() << "An error occurred during parsing:" << e.what();
        return 1;
    } catch (...) {
        qCritical() << "An unknown error occurred during parsing";
        return 1;
    }

    return 0;
}
