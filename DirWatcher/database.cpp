#include "database.h"
#include <QDebug>
#include <QUuid>
#include <QDesktopServices>
#include <QDir>

DataBase::DataBase()
{
    m_isDbOpen = createConnection();
}

DataBase::~DataBase()
{
    if(m_isDbOpen)
    {
        m_db.close();
    }
}
//建立一个数据库连接
bool DataBase::createConnection()
{
    //以后就可以用"classes_db"与数据库进行连接了

    //与数据库建立连接
    if (QSqlDatabase::contains("classes_db"))
    {
        m_db = QSqlDatabase::database("classes_db");
    }
    else
    {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "classes_db");
    }
    //设置数据库名

    QString dbFilePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    QDir *dbDir = new QDir;
    bool isDbDirExist = dbDir->exists(dbFilePath);
    if(isDbDirExist)
    {
        qDebug() << "文件夹已经存在！" << endl;
    }
    else
    {
        bool ok = dbDir->mkdir(dbFilePath);
        if( ok )
        {
            qDebug() << "文件夹创建成功！" << endl;
        }
    }
    dbFilePath = dbFilePath.append("/classes_info.db");
    qDebug() << "数据库地址：" << dbFilePath << endl;
    m_db.setDatabaseName(dbFilePath);
    if( !m_db.open())
    {
        qDebug() << "无法建立数据库连接";
        return false;
    }
    qDebug() << "建立数据库连接成功！";
    return true;
}

//创建数据库表
bool DataBase::createTable()
{
    if(!m_db.isOpen())
    {
        createConnection();
    }

    QSqlQuery query(m_db);
    bool success = query.exec("create table if not exists classes_table("
                              "[id] varchar PRIMARY KEY,"
                              "[file_path] varchar,[file_name] varchar,"
                              "[class_name] varchar,[identify_name] varchar,"
                              "[file_type] int,[identify_type] int)");
    if(success)
    {
        qDebug() << QObject::tr("数据库表创建成功！\n");
        return true;
    }
    else
    {
        qDebug() << QObject::tr("数据库表创建失败！\n");
        return false;
    }
}

//向数据库中插入记录
bool DataBase::insertRecord(ClassModel classModel)
{
    if(!m_db.isOpen())
    {
        createConnection();
    }

    QSqlQuery query(m_db);
    query.prepare("insert into classes_table values(:id,:file_path,:file_name,:class_name,:identify_name,:file_type,:identify_type)");

    QUuid id = QUuid::createUuid();
    QString strId = id.toString();
    query.bindValue(":id", strId);
    query.bindValue(":file_path", classModel.filePath.c_str());
    query.bindValue(":file_name", classModel.fileName.c_str());
    query.bindValue(":class_name", classModel.className.c_str());
    query.bindValue(":identify_name", classModel.identifyName.c_str());
    query.bindValue(":file_type", classModel.fileType);
    query.bindValue(":identify_type", classModel.identifyType);

    bool success=query.exec();
    if(!success)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText() << QString(QObject::tr("插入失败"));
        return false;
    }
    return true;
}

//查询所有信息
bool DataBase::queryAll()
{
    if(!m_db.isOpen())
    {
        createConnection();
    }

    QSqlQuery query(m_db);
    query.exec("select * from automobil");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("automobil表字段数：" ) << rec.count();

    while(query.next())
    {
        for(int index = 0; index < 10; index++)
            qDebug() << query.value(index) << " ";
        qDebug() << "\n";
    }

    return true;
}

//根据ID删除记录
bool DataBase::deleteById(int id)
{
    if(!m_db.isOpen())
    {
        createConnection();
    }

    QSqlQuery query(m_db);
    query.prepare(QString("delete from automobil where id = %1").arg(id));
    if(!query.exec())
    {
        qDebug() << "删除记录失败！";
        return false;
    }
    return true;
}

//根据ID更新记录
bool DataBase::updateById(int id)
{
    if(!m_db.isOpen())
    {
        createConnection();
    }

    QSqlQuery query(m_db);
    query.prepare(QString("update automobil set attribute=?,type=?,"
                             "kind=?, nation=?,"
                             "carnumber=?, elevaltor=?,"
                             "distance=?, oil=?,"
                             "temperature=? where id=%1").arg(id));

     query.bindValue(0,"四轮");
     query.bindValue(1,"轿车");
     query.bindValue(2,"富康");
     query.bindValue(3,rand()%100);
     query.bindValue(4,rand()%10000);
     query.bindValue(5,rand()%300);
     query.bindValue(6,rand()%200000);
     query.bindValue(7,rand()%52);
     query.bindValue(8,rand()%100);

     bool success=query.exec();
     if(!success)
     {
          QSqlError lastError = query.lastError();
          qDebug() << lastError.driverText() << QString(QObject::tr("更新失败"));
     }
    return true;
}

//排序
bool DataBase::sortById()
{
    if(!m_db.isOpen())
    {
        createConnection();
    }

    QSqlQuery query(m_db);
    bool success=query.exec("select * from automobil order by id desc");
    if(success)
    {
        qDebug() << QObject::tr("排序成功");
        return true;
    }
    else
    {
        qDebug() << QObject::tr("排序失败！");
        return false;
    }
}
