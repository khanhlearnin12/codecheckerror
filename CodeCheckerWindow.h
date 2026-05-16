//bảo vệ include 
#ifndef CODECHECKERWINDOW_H
#define CODECHECKERWINDOW_H

#include <QWidget> // class sẽ được kế thừa từ nó 
#include <QString>
#include <QList>

struct ToolsResult{
    QString toolName; // Tên tool (cppcheck, clang-tidy...)
    bool passed;      //true Pass, false trả về false
    QString errorlog; //chi tiết lỗi ra từ terminal
};

//Khai bao tool tự làm 
class toolColab{
    public:
        QString runAllCheck(const QString &filePath, const QString &reportFilePath);

    private:
        // thêm tools vào khoản này
        ToolsResult runCppCheck(const QString &filePath);
        ToolsResult runClangTidy(const QString &filePath);
        //...
};

//khai báo trước
class QTextEdit;
class QPushButton;

// khai bao loip macro Q_object
class CodeCheckerWindow : public QWidget {
    Q_OBJECT //kích hoạt MOC (micro object compiler)

// hàm tạo và hàm hủy 
public:
    explicit CodeCheckerWindow(QWidget *parent = nullptr); //khởi tạo mặc định không có parent
    ~CodeCheckerWindow();
    
    // thêm function từ ngoài vào đây 
    void LoadFileFormPath(const QString &filePath);

private slots: // hàm xử lý sự kiện
    void onUploadClicked();
    void onCheckClicked();

// thành phần nội bộ
private:
    void setupUI(); 
        
    //hàm phụ trợ giao diện 
    QTextEdit *codeEditor;
    QTextEdit *outputConsole;
    QPushButton *btnUpload;
    QPushButton *btnCheck;
    
    //tools minh tu build
    toolColab myTools;  
};

#endif 