#include <iostream>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

using namespace std;

void printTable(const QString& tableName) {
    cout << "\n========== " << tableName.toStdString() << " 表 ==========" << endl;
    
    QSqlQuery query;
    if (!query.exec("SELECT * FROM " + tableName)) {
        cout << "查询失败: " << query.lastError().text().toStdString() << endl;
        return;
    }
    
    // 获取列数
    int columnCount = query.record().count();
    
    // 打印列标题
    for (int i = 0; i < columnCount; i++) {
        cout << query.record().fieldName(i).toStdString();
        if (i < columnCount - 1) cout << " | ";
    }
    cout << endl;
    
    // 打印分隔线
    for (int i = 0; i < columnCount; i++) {
        cout << "----------";
        if (i < columnCount - 1) cout << "---";
    }
    cout << endl;
    
    // 打印数据行
    int rowCount = 0;
    while (query.next()) {
        for (int i = 0; i < columnCount; i++) {
            QString value = query.value(i).toString();
            if (value.length() > 30) {
                value = value.left(27) + "...";
            }
            cout << value.toStdString();
            if (i < columnCount - 1) cout << " | ";
        }
        cout << endl;
        rowCount++;
    }
    cout << "总共 " << rowCount << " 行数据" << endl;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("railway_system.db");
    
    if (!db.open()) {
        cout << "无法打开数据库: " << db.lastError().text().toStdString() << endl;
        return -1;
    }
    
    cout << "=== 铁路系统数据库查看器 ===" << endl;
    cout << "数据库文件: railway_system.db" << endl;
    
    // 获取所有表名
    QSqlQuery query("SELECT name FROM sqlite_master WHERE type='table'");
    QStringList tables;
    while (query.next()) {
        tables << query.value(0).toString();
    }
    
    cout << "数据库包含以下表: ";
    for (const QString& table : tables) {
        cout << table.toStdString() << " ";
    }
    cout << endl;
    
    // 显示每个表的内容
    for (const QString& table : tables) {
        printTable(table);
    }
    
    cout << "\n数据库查看完成!" << endl;
    
    return 0;
} 