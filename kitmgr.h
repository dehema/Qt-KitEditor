#ifndef KITMGR_H
#define KITMGR_H

#include <QString>
#include <QList>
#include <QByteArray>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "Single.h"
#include "kitmodel.h"

//指标类型
enum NormType {
  //未定义/禁用0
  unDefine,
  //普通指标1
  normal,
  //2内质控
  internal,
};

class KitMgr:public Single<KitMgr>
{
public:
    KitMgr();

    QList<QString> normTypeStr = {"无", "普通", "内质控"};
    const QString configPath = "E:/Project/QT/KitEditor/data/Default.json";
    const QString filePath = "E:/Project/QT/KitEditor/data/kit/E015/";


    KitModel getProcedureListByData(QByteArray _byteArray);
    QList<SpoolModel> getSpoolModelsByValue(QJsonValue _jsonValue);

};

#endif // KITMGR_H
