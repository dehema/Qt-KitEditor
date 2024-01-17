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

    QList<QString> normTypeStr;
    QString configPath;
    QString filePath;


    KitModel getProcedureListByData(QByteArray _byteArray);
    QList<SpoolModel> getSpoolModelsByValue(QJsonValue _jsonValue);
    void sortKitBySpecimenNo(KitModel kitModel);
    void sortSpoolBySpecimenNo(QList<SpoolModel> spoolModel);
    //根据指标PoolID排序
    void sortKitByPoolIndex(KitModel kitModel);
    void sortSpoolByPoolIndex(QList<SpoolModel> spoolModel);
};

#endif // KITMGR_H
