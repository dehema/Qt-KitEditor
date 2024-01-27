#ifndef KITMGR_H
#define KITMGR_H

#include <QString>
#include <QList>
#include <QByteArray>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QRegExp>
#include "single.h"
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

struct KitEditorParamsList
{
    QMap<QString,int> map;
};

struct KitEditorParams
{
    QString preview;
    QMap<QString,KitEditorParamsList> list;
    KitEditorParams()
    {
        preview = QString();
    }
};

class KitMgr:public Single<KitMgr>
{
public:
    KitMgr();

    QList<QString> normTypeStr;
    KitModel kitConfig;
    QString kitName;
    QString machineID;

    KitModel getProcedureListByData(QByteArray _byteArray);
    QList<SpoolModel> getSpoolModelsByValue(QJsonValue _jsonValue);
    //根据指标SpecimenNo排序
    void sortKitBySpecimenNo(KitModel& kitModel);
    void sortSpoolBySpecimenNo(QList<SpoolModel>& spoolModel);
    //根据指标PoolID排序
    void sortKitByPoolIndex(KitModel& kitModel);
    void sortSpoolByPoolIndex(QList<SpoolModel>& spoolModel);
    //根据配置表里的SpecimenNo字段进行排序
    void sortPoolByConfigSpecimenNo(KitModel& kitModel);
    KitEditorParams getKitEditorParams(QString _params);

    QString getKitJsonStr(KitModel _kitModel);
    QJsonArray getSpoolJsonArray(QList<SpoolModel> _spoolModel);
    void loadKitConfig();
    SpoolModel getSpoolFromConfigByAbbrName(QString _abbrName);

    QString getConfigPath();
    QString getPublishPath();
    QList<QString> checkKitTips(KitModel _kitModel);

private:
    QString configPath;
    QString filePath;
};

#endif // KITMGR_H
