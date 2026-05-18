#include "CodeCheckerWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QProcess>



// Constructor
CodeCheckerWindow::CodeCheckerWindow(QWidget *parent) : QWidget(parent) {
    setupUI();
    
    // Kết nối Signal (sự kiện) với Slot (hàm xử lý)
    connect(btnUpload, &QPushButton::clicked, this, &CodeCheckerWindow::onUploadClicked);
    connect(btnCheck, &QPushButton::clicked, this, &CodeCheckerWindow::onCheckClicked);
    connect(btnFormatCode, &QPushButton::clicked, this , &CodeCheckerWindow::onFormatClicked);
}

// Destructor
CodeCheckerWindow::~CodeCheckerWindow() {
    // Qt tự động giải phóng bộ nhớ của các widget con (childrủ công ở đâen) nên không cần delete thy
}

void CodeCheckerWindow::setupUI() {
    setWindowTitle("C/C++ Code Checker");
    resize(1000, 600);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    // ================= PHẦN BÊN TRÁI: Nhập Code =================
    QWidget *leftWidget = new QWidget(splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnUpload = new QPushButton("Upload File");
    btnCheck = new QPushButton("Check the code");
    btnFormatCode = new QPushButton("Format My Code");
    
    buttonLayout->addWidget(btnUpload);
    buttonLayout->addWidget(btnCheck);
    buttonLayout->addWidget(btnFormatCode); 
    buttonLayout->addStretch();

    codeEditor = new QTextEdit();
    codeEditor->setPlaceholderText("Input C/C++ code here or u can Upload file...");
    QFont font = codeEditor->font();
    font.setFamily("Courier New");
    font.setPointSize(16);
    codeEditor->setFont(font);

    leftLayout->addLayout(buttonLayout);
    leftLayout->addWidget(codeEditor);

    // ================= PHẦN BÊN PHẢI: Output Lỗi =================
    outputConsole = new QTextEdit(splitter);
    outputConsole->setReadOnly(true);
    outputConsole->setPlaceholderText("Output from clang-tidy / cppcheck will be appear here...");
    outputConsole->setStyleSheet("background-color: #1e1e1e; color: #d4d4d4; font-family: monospace;");
    QFont consFont = outputConsole->font();
    font.setPointSize(16);
    font.setFamily("Courier New");
    outputConsole->setFont(font);

    splitter->addWidget(leftWidget);
    splitter->addWidget(outputConsole);
    splitter->setSizes({600, 400});

    mainLayout->addWidget(splitter);
}


// Hàm xử lý Upload
void CodeCheckerWindow::onUploadClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, 
        "Chose file C++", "", "C/C++ Files (*.cpp *.c *.h *.hpp);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            codeEditor->setPlainText(in.readAll());
            file.close();
            outputConsole->append("Already Download: " + fileName);
        } else {
            outputConsole->append("Error: Can not openning file " + fileName);
        }
    }
}

// Hàm xử lý Check Code
void CodeCheckerWindow::onCheckClicked() {
    outputConsole->clear();
    outputConsole->append("CODE IS READY...");
    
    QString currentCode = codeEditor->toPlainText();
    if (currentCode.isEmpty()) {
        outputConsole->append("ERROR: NO CODE TO CHECK!");
        return;
    }

    // 1. Tạo một file tạm và lưu chữ trên màn hình vào file đó
    QString tempFilePath = "temp_code.cpp";
    QFile file(tempFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << currentCode;
        file.close();
    } else {
        outputConsole->append("ERROR: CANNOT CREATE TEMP FILE!");
        return;
    }

    QString reportFilePath = "errorreport.txt";
    
    QString fullReport = myTools.runAllCheck(tempFilePath, reportFilePath);

    outputConsole->setPlainText(fullReport);
 
    outputConsole->append("\n>> Already save error report at: " + reportFilePath);
}

// Format lại code 
void CodeCheckerWindow::onFormatClicked() {
    outputConsole->append(">> Make your code in good format...");

    QString currentCode = codeEditor->toPlainText();
    if (currentCode.isEmpty()) return;

    // 1. Lưu code hiện tại xuống file tạm
    QString tempFilePath = "temp_code.cpp";
    QFile fileWrite(tempFilePath);
    if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&fileWrite);
        out << currentCode;
        fileWrite.close();
    }

    // 2. Gọi logic format file
    ToolsResult result = myTools.onMagicFormat(tempFilePath);

    if (result.passed) {
        // 3. QUAN TRỌNG NHẤT: Đọc lại file vừa được format và ghi đè lên màn hình
        QFile fileRead(tempFilePath);
        if (fileRead.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&fileRead);
            codeEditor->setPlainText(in.readAll()); // Code trên màn hình sẽ giật một cái và đẹp ngay lập tức!
            fileRead.close();
            outputConsole->append(">> Magic Format: Format Finished!");
        }
    } else {
        // Báo lỗi nếu clang-format thất bại
        outputConsole->append(result.errorlog);
    }
}

//lấy File từ Terminal 
void CodeCheckerWindow::LoadFileFormPath(const QString &filePath){
    QFile file(filePath);
    if(file.open(QIODevice::Text | QIODevice::ReadOnly)){
        QTextStream in(&file);
        codeEditor->setPlainText(in.readAll());
        file.close();
        outputConsole->append("Already Loaded File: " + filePath);
    }else{
        outputConsole->append("Error: Can not load file " + filePath + "form terminal");
    }
}

// compile code và output compile file 
QString toolColab::CompileCode(const QString &filePath){
    QProcess process;
    QString executablePath = "./outputfile";
    QStringList args;
    args << "-Wall"
         << "-g"
         << filePath 
         << "-o"
         << executablePath;
    process.start("g++", args);
    process.waitForFinished();
    
    // Kiểm tra xem g++ có chạy thành công (Exit Code == 0) hay không 
    if (process.exitCode() != 0) return ""; // tra rỏng nếu bị lỗi biên dịch 

// ==============================================================
// KHU VỰC DÀNH RIÊNG CHO MAC: Vượt rào bảo mật SIP
// ==============================================================
#ifdef Q_OS_MAC
    // 1. Tạo file XML chứa "Thẻ bài" cho phép Debug (get-task-allow)
    QFile entFile("entitlements.plist");
    if (entFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&entFile);
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
            << "<plist version=\"1.0\">\n<dict>\n"
            << "<key>com.apple.security.get-task-allow</key>\n<true/>\n"
            << "</dict>\n</plist>";
        entFile.close();
    }

    // 2. Dùng công cụ 'codesign' của Apple để đóng dấu thẻ bài vào file thực thi
    QProcess signProcess;
    QStringList signArgs;
    signArgs << "-s" << "-" << "-f" << "--entitlements" << "entitlements.plist" << executablePath;
    
    signProcess.start("codesign", signArgs);
    signProcess.waitForFinished();
#endif
// ==============================================================

    return executablePath;
}

//chạy cppcheck 
ToolsResult toolColab::runCppCheck(const QString &filePath){
    QProcess process;
    
    QStringList args;
    
    args << "--quiet"
         << "--enable=warning,style,performance,portability"
         << "--suppress=missingIncludeSystem"
         << filePath;
    
    process.start("cppcheck", args);
    process.waitForFinished(); //doi chay xong

    //thể hiện output bị sai 
    QString output = process.readAllStandardError().trimmed();
    
    bool isPass = output.isEmpty();

    return {"Cppcheck" ,isPass, output};
}

//chay clang-tidy
ToolsResult toolColab::runClangTidy(const QString &filePath){
    QProcess process;
    //lệnh để chạy clang-tidy
    QStringList args;
    args << filePath 
        << "-checks=bugprone-*,modenize-*,readability-*" 
        << "-header-filter=^$"
        << "--"
        << "-std=c++17";
    process.start("clang-tidy",args);

    if (!process.waitForFinished()){
        if(process.exitStatus() == QProcess::CrashExit){
            return {"Clang-Tidy", false ,"Process Stop(Could be not enough Ram/Killed)."};
        }
    }

    // Đọc báo cáo CODE từ StandardOutput
    QString output = process.readAllStandardError().trimmed();
    // Đọc thêm về error khi tools crash
    QString systemError = process.readAllStandardError().trimmed();
    if(!systemError.isEmpty()) output += "\n [System Info:]: " + systemError;

    bool isPass =  output.isEmpty() ||(!output.contains("Warning:") && !output.contains("Error:"));
    
    return {"ClangTidy" ,isPass, output};
}

//flawfinder tools
ToolsResult toolColab::runflawfinder(const QString &filePath){
    QProcess process;
    
    QStringList args;
    args << "--dataonly" 
         << "--minlevel=2" 
         << filePath;
    
    process.start("flawfinder",args);
    if(!process.waitForFinished()) 
        return {"Flawfinder", false, "Error: Can not run FlawFinder."};

    QString output = process.readAllStandardError().trimmed();

    // Logic kiểm tra Pass cho Flawfinder
    bool isPass = !output.contains("  [2]") && 
                    !output.contains("  [3]") && 
                    !output.contains("  [4]") && 
                    !output.contains("  [5]");

    if(output.isEmpty()) isPass = true;

    return {"Flawfinder", isPass, output};
}

//tính về time complie find   
ToolsResult toolColab::runMemoryCheck(const QString &filePath){

    QString executableFile = CompileCode(filePath);

    if(executableFile.isEmpty()) return {"Memory Check", false, "Error: Can not run Valgrind (Compiler Error)"};

    QFileInfo exeInfo(executableFile);
    QString absoluteExePath = exeInfo.absoluteFilePath();

    QProcess process;
    QStringList args;
    QString actualToolName;

    // tự check hệ điều hành 
#ifdef Q_OS_MAC // mac using leaks 
    actualToolName = "Apple Leaks";
    QString bashCommand = QString("export MallocStackLogging=1 && leaks -atExit -- \"%1\"").arg(absoluteExePath);

    args << "-c" << bashCommand;
    process.start("/bin/bash", args);
#else //valgrind
    actualToolName = "Valgrind";
    args << "--leak-check=full" << executableFile;
    process.start("valgrind", args);

#endif
    process.waitForFinished();
    
    QString output = process.readAllStandardError().trimmed();
    QString errorLog = process.readAllStandardError().trimmed();
    QString combinedLog = output + "\n\n" + errorLog;

    if (combinedLog.trimmed().isEmpty()) {
        return {actualToolName, false, "Lỗi: Tool không xuất ra dữ liệu. (Exit code: " + QString::number(process.exitCode()) + ")"};
    }
    
#ifdef Q_OS_MAC
    // Với Apple Leaks, nếu an toàn nó sẽ in ra "0 leaks for 0 total leaked bytes"
    bool isPass = combinedLog.contains("0 leaks for 0 total leaked bytes");

    // Mẹo nhỏ: Xóa bớt mấy dòng cảnh báo rác của macOS cho giao diện sạch đẹp (Nếu Pass)
    if (isPass) {
        combinedLog = "Mọi thứ hoàn hảo! Không phát hiện rò rỉ bộ nhớ (Memory Leak).\n" + output;
    }
#else 
    //với valgrind 
    bool isPass = combinedLog.contains("ERROR SUMMARY: 0 errors");

#endif
    return {actualToolName, isPass, combinedLog};
}

ToolsResult toolColab::runclangFormat(const QString &filePath){
    QProcess process;
    
    QStringList args;
    args << "--dry-run" 
         << "-Werror"
         << "--style=Google"
         << filePath;

    process.start("clang-format",args);
    process.waitForFinished();

    QString output = process.readAllStandardError().trimmed();
    
    bool isPass = output.isEmpty();
    
    return {"Clang-Format", isPass , output};
}

ToolsResult toolColab::onMagicFormat(const QString &filePath){
    QProcess process;
    
    QStringList args;
    
    args << "-i" 
         << "--style=Google"
         << filePath;

    process.start("clang-format", args);
    process.waitForFinished();
    
   // Kiểm tra xem có lỗi hệ thống không
    if (process.exitCode() != 0) {
        return {"Clang-Format", false, "Lỗi: Không thể format đoạn code này (Có thể do lỗi cú pháp quá nặng)."};
    }
    return {"Clang-Format", true, "Đã format file thành công!"};
}

// --- 2. Hàm tổng hợp (Pipeline) thực thi sơ đồ của bạn ---
QString toolColab::runAllCheck(const QString &sourceFilePath, const QString &reportFilePath) {
    // Chạy tất cả các tool và gom vào một danh sách
    QList<ToolsResult> results;
    results.append(runclangFormat(sourceFilePath));
    results.append(runflawfinder(sourceFilePath));
    results.append(runCppCheck(sourceFilePath));
    results.append(runClangTidy(sourceFilePath));
    results.append(runMemoryCheck(sourceFilePath));
    
    
    // Thêm các tool khác vào đây...

    QString finalReport = "";
    bool allPass = true; // Cờ theo dõi trạng thái tổng

    // PHẦN 1: IN DANH SÁCH PASS/ERROR (Giống đoạn giữa sơ đồ)
    finalReport += "========== TOOLS STATUS ==========\n";
    for (const ToolsResult &res : results) {
        if (res.passed) {
            finalReport += res.toolName + ": PASS\n";
        } else {
            finalReport += res.toolName + ": ERROR\n";
            allPass = false; // Chỉ cần 1 tool báo lỗi, cờ này sẽ thành false
        }
    }
    finalReport += "=========================================\n\n";

    // PHẦN 2: CHI TIẾT CÁC LỖI CẦN SỬA
    if (!allPass) {
        finalReport += "========== ERROR DETAIL ==========\n";
        for (const ToolsResult &res : results) {
            if (!res.passed) {
                finalReport += "[" + res.toolName + " found the error]:\n";
                finalReport += res.errorlog + "\n\n";
            }
        }
    }

    // PHẦN 3: KẾT LUẬN CUỐI CÙNG (Giống 2 mũi tên rẽ nhánh ở cuối sơ đồ)
    finalReport += "========== CONCLUSION ==========\n";
    if (allPass) finalReport += "CONGRATULATION: ALL PASS\n";
    else finalReport += "THERE ERROR ABOVE NEED TO BE FIX\n";

    // PHẦN 4: LƯU RA FILE errorreport.txt
    QFile reportFile(reportFilePath);
    if (reportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&reportFile);
        out << finalReport;
        reportFile.close();
    }

    // Trả báo cáo này về cho UI hiển thị
    return finalReport;
}
