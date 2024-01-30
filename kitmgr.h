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
using namespace std;
class KitMgr
{
public:
    static KitMgr& ins()
    {
        static KitMgr instance;   //函数静态变量可以实现延时构造。
        return instance;
    }

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
    //根据通道数据获取到对应的显示位置
    int getSpecimenNoByConfig(SpoolModel _spool);
    KitEditorParams getKitEditorParams(QString _params);

    QString getKitJsonStr(KitModel _kitModel);
    QJsonArray getSpoolJsonArray(QList<SpoolModel> _spoolModel);
    void loadKitConfig();
    SpoolModel getSpoolFromConfigByAbbrName(QString _abbrName);

    QString getConfigPath();
    QString getPublishPath();
    QList<QString> checkKitTips(KitModel _kitModel);
    //颜色列表
    QMap<QString, QString> getColorOption();
    //获取相反的颜色
    QString getInverseFontColor(QString _color);

private:
    QString configPath;
    QString filePath;
    QString colorFilePath;
    QMap<QString, QString> spoolColorMap;
};

#endif // KITMGR_H
