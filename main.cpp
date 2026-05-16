#include <QApplication>
#include <QStringList>
#include "CodeCheckerWindow.h"
 

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    
    //chạy window để nhập code
    CodeCheckerWindow window;
    window.show();

    QStringList args = app.arguments(); 
    
    if(args.size() > 1){
        QString filePath = args.at(1);
        window.LoadFileFormPath(filePath);
    }
        
    return app.exec();
}