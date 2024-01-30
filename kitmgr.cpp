#include "kitmgr.h"

KitMgr::KitMgr()
{
    normTypeStr << QString::fromLocal8Bit("无");
    normTypeStr << QString::fromLocal8Bit("普通");
    normTypeStr << QString::fromLocal8Bit("内质控");
    configPath = QString("E:/Project/Qt/KitEditor/data/kit/%1/MultiFluoInfo.json");
    filePath = QString("E:/Project/QT/KitEditor/data/kit/%1/");
    colorFilePath = QString("E:/Project/QT/KitEditor/data/colors.json");
}

QString KitMgr::getConfigPath()
{
    return configPath.arg(machineID);
}

QString KitMgr::getPublishPath()
{
    return filePath.arg(machineID);
}

void KitMgr::loadKitConfig()
{
    QFile file(getConfigPath());
    if(file.open(QFile::ReadOnly))
    {
        QByteArray byteArray = file.readAll();
        kitConfig = getProcedureListByData(byteArray);
        //探针染料选项
        for(SpoolModel& spoolModel:kitConfig.spoolList)
        {
            for(QString dye:spoolModel.fullName.split("/"))
            {
                spoolModel.dyeList.append(dye);
            }
        }
        sortKitBySpecimenNo(kitConfig);
    }
}

SpoolModel KitMgr::getSpoolFromConfigByAbbrName(QString _abbrName)
{
    for(SpoolModel spoolModel:kitConfig.spoolList)
    {
        if(spoolModel.dyeList.contains(_abbrName))
        {
            return spoolModel;
        }
    }
    return SpoolModel();
}

KitModel KitMgr::getProcedureListByData(QByteArray _byteArray)
{
    KitModel kit;

    QJsonDocument jsonDocu = QJsonDocument::fromJson(_byteArray.toStdString().data());
    if(jsonDocu.isObject())
    {
        QJsonObject obj_root = jsonDocu.object();
        kit.abbrName = obj_root.value("AbbrName").toString();
        kit.barcode = obj_root.value("Barcode").toString();
        kit.dosage = obj_root.value("Dosage").toInt();
        kit.filetype = obj_root.value("Filetype").toString();
        kit.fullName = obj_root.value("FullName").toString();
        kit.distillFile = obj_root.value("DistillFile").toString();
        kit.ampFile = obj_root.value("AmpFile").toString();
        kit.spoolList = getSpoolModelsByValue(obj_root.value("SpoolList"));
        kit.positiveSpoolList = getSpoolModelsByValue(obj_root.value("PositiveList"));
        kit.negativeSpoolList = getSpoolModelsByValue(obj_root.value("NegativeList"));
    }
    return kit;
}

void KitMgr::sortPoolByConfigSpecimenNo(KitModel& _kitModel)
{
    for(int i = 0;i < kitConfig.spoolList.length();i++)
    {
        SpoolModel spoolConfig = kitConfig.spoolList[i];
        for(int j = i;j < _kitModel.spoolList.length();j++)
        {
            SpoolModel spoolData = _kitModel.spoolList[j];
            if(spoolConfig.dyeList.contains(spoolData.abbrName))
            {
                _kitModel.spoolList.removeAt(j);
                _kitModel.spoolList.insert(i,spoolData);
                continue;
            }
        }

    }
}

int KitMgr::getSpecimenNoByConfig(SpoolModel _spoolData)
{
    for(SpoolModel spoolModel:kitConfig.spoolList)
    {
        if(spoolModel.dyeList.contains(_spoolData.abbrName))
        {
            return spoolModel.specimenNo;
        }
    }
    return 0;
}

QList<SpoolModel> KitMgr::getSpoolModelsByValue(QJsonValue _jsonValue)
{
    QList<SpoolModel> list;
    QJsonArray contentArray = _jsonValue.toArray();
    for(int i = 0;i < contentArray.count();i++)
    {
        SpoolModel spool;
        QJsonObject obj = contentArray[i].toObject();
        spool.abbrName = obj.take("AbbrName").toString();
        spool.fullName = obj.take("FullName").toString();
        spool.ctMin = obj.take("CtMin").toInt();
        spool.ctMax = obj.take("CtMax").toInt();
        spool.datumMin = obj.take("DatumMin").toInt();
        spool.datumMax = obj.take("DatumMax").toInt();
        spool.threshold = obj.take("Threshold").toInt();
        spool.curveColor = obj.take("CurveColor").toString();
        spool.specimenNo = obj.take("SpecimenNo").toInt();
        spool.specimenType = obj.take("SpecimenType").toInt();
        spool.poolIndex = obj.take("PoolIndex").toInt();
        list.append(spool);
    }
    qSort(list.begin(), list.end(), [this](SpoolModel a,SpoolModel b){return a.specimenNo < b.specimenNo;});
    return list;
}

void KitMgr::sortKitBySpecimenNo(KitModel& _kitModel)
{
    sortSpoolBySpecimenNo(_kitModel.spoolList);
    sortSpoolBySpecimenNo(_kitModel.positiveSpoolList);
    sortSpoolBySpecimenNo(_kitModel.negativeSpoolList);
}

void KitMgr::sortSpoolBySpecimenNo(QList<SpoolModel>& _spoolList)
{
    for(int i = 0;i<_spoolList.length();i++)
    {
        for(int j = i;j<_spoolList.length();j++)
        if(_spoolList[j].specimenNo==i)
        {
            SpoolModel spoolModel = _spoolList.at(j);
            _spoolList.removeAt(j);
            _spoolList.insert(i,spoolModel);
        }
    }
}

void KitMgr::sortKitByPoolIndex(KitModel& _kitModel)
{
    sortSpoolByPoolIndex(_kitModel.spoolList);
    sortSpoolByPoolIndex(_kitModel.positiveSpoolList);
    sortSpoolByPoolIndex(_kitModel.negativeSpoolList);
}

void KitMgr::sortSpoolByPoolIndex(QList<SpoolModel>& _spoolList)
{
    for(int i = 0;i<_spoolList.length();i++)
    {
        for(int j = i;j<_spoolList.length();j++)
        if(_spoolList[j].poolIndex==i)
        {
            SpoolModel spoolModel = _spoolList[j];
            _spoolList.removeAt(j);
            _spoolList.insert(i,spoolModel);
        }
    }
}

KitEditorParams KitMgr::getKitEditorParams(QString _paramsStr)
{
    KitEditorParams params;
    QByteArray byteArray = _paramsStr.toLatin1();
    QJsonDocument jsonDocu = QJsonDocument::fromJson(byteArray);
    if(jsonDocu.isObject())
    {
        QJsonObject obj_root = jsonDocu.object();
        params.preview = obj_root.value("preview").toString();
        QJsonObject objList = obj_root.value("list").toObject();
        QStringList keyList = objList.keys();
        for(QString key:keyList)
        {
            KitEditorParamsList list;
            QJsonObject objMap = objList.value(key).toObject();
            for(int j = 0;j < objMap.length();j++)
            {
                list.map[key] = objMap.value(key).toInt();
            }
            params.list[key] = list;
        }
    }
    return params;
}

QString KitMgr::getKitJsonStr(KitModel _kitModel)
{
    QJsonObject obj_root;
    obj_root.insert("AbbrName",_kitModel.abbrName);
    obj_root.insert("Barcode",_kitModel.barcode);
    obj_root.insert("Dosage",_kitModel.dosage);
    obj_root.insert("Filetype",_kitModel.filetype);
    obj_root.insert("FullName",_kitModel.fullName);
    obj_root.insert("DistillFile",_kitModel.distillFile);
    obj_root.insert("AmpFile",_kitModel.ampFile);
    obj_root.insert("SpoolList", getSpoolJsonArray(_kitModel.spoolList));
    obj_root.insert("PositiveList", getSpoolJsonArray(_kitModel.positiveSpoolList));
    obj_root.insert("NegativeList", getSpoolJsonArray(_kitModel.negativeSpoolList));
    QJsonDocument doc(obj_root);
    QString jsonStr = doc.toJson();
    return jsonStr;
}

QJsonArray KitMgr::getSpoolJsonArray(QList<SpoolModel> _spoolModelList)
{
    QJsonArray arraySpool;
    for(SpoolModel spool:_spoolModelList)
    {
        QJsonObject objSpool;
        objSpool.insert("AbbrName",spool.abbrName);
        objSpool.insert("FullName",spool.fullName);
        objSpool.insert("DatumMin",spool.datumMin);
        objSpool.insert("DatumMax",spool.datumMax);
        objSpool.insert("CtMin",spool.ctMin);
        objSpool.insert("CtMax",spool.ctMax);
        objSpool.insert("Threshold",spool.threshold);
        objSpool.insert("CurveColor",spool.curveColor);
        objSpool.insert("SpecimenNo",spool.specimenNo);
        objSpool.insert("SpecimenType",spool.specimenType);
        objSpool.insert("PoolIndex",spool.poolIndex);
        arraySpool.append(objSpool);
    }
    return arraySpool;
}

QList<QString> KitMgr::checkKitTips(KitModel _kitModel)
{
    sortPoolByConfigSpecimenNo(_kitModel);
    QList<QString> tipsList;
    if(_kitModel.abbrName.isEmpty())
        tipsList.append(QString::fromLocal8Bit("试剂盒名称不能为空"));
    if(!_kitModel.abbrName.isEmpty() && _kitModel.abbrName.at(0).isNumber())
        tipsList.append(QString::fromLocal8Bit("试剂盒名称不能是数字开头"));
    QRegExp regLetterNumber("[a-zA-Z0-9]+$");
    if(!regLetterNumber.exactMatch(_kitModel.abbrName))
        tipsList.append(QString::fromLocal8Bit("试剂盒名称只能由字母和数字组成"));
    if(_kitModel.abbrName.length() > 20)
        tipsList.append(QString::fromLocal8Bit("试剂盒名称长度不能超过20位"));
    if(_kitModel.fullName.isEmpty())
        tipsList.append(QString::fromLocal8Bit("试剂盒全称不能为空"));
    if(_kitModel.fullName.length() > 40)
        tipsList.append(QString::fromLocal8Bit("试剂盒全称长度不能超过40位"));
    if(_kitModel.dosage <= 0)
        tipsList.append(QString::fromLocal8Bit("反应体系必须大于0"));
    if(_kitModel.ampFile.isEmpty())
        tipsList.append(QString::fromLocal8Bit("未选择温控流程"));
    for(int i = 0;i < _kitModel.spoolList.length();i++)
    {
        SpoolModel spoolData = _kitModel.spoolList[i];
        int index = i + 1;
        if(spoolData.fullName.isEmpty())
            tipsList.append(QString::fromLocal8Bit("第%1通道指标不能为空").arg(index));
        if(!spoolData.fullName.isEmpty() && spoolData.abbrName.at(0).isNumber())
            tipsList.append(QString::fromLocal8Bit("第%1通道指标不能是数字开头").arg(index));
        QRegExp regLetterNumberUnderline("^[A-Za-z0-9_.]+$");
        if(!regLetterNumberUnderline.exactMatch(spoolData.abbrName))
            tipsList.append(QString::fromLocal8Bit("第%1通道指标只能由字母、小数点、下划线和数字组成").arg(index));
        if(spoolData.fullName.length() > 40)
            tipsList.append(QString::fromLocal8Bit("第%1通道指标长度不能超过40位").arg(index));
        if(spoolData.datumMin <= 0)
            tipsList.append(QString::fromLocal8Bit("第%1通道基线起点必须大于0").arg(index));
        if(spoolData.datumMax <= spoolData.datumMin)
            tipsList.append(QString::fromLocal8Bit("第%1通道基线终点必须大于基线起点").arg(index));
        if(spoolData.ctMin <= spoolData.datumMax)
            tipsList.append(QString::fromLocal8Bit("第%1通道Ct下限必须大于基线终点").arg(index));
        if(spoolData.ctMax <= spoolData.ctMin)
            tipsList.append(QString::fromLocal8Bit("第%1通道Ct上限必须大于Ct下限").arg(index));
        for(int j = i + 1;j < _kitModel.spoolList.length();j++)
        {
            if(spoolData.fullName == _kitModel.spoolList[j].fullName)
            tipsList.append(QString::fromLocal8Bit("第%1通道名称不能与第%2通道名称一致").arg(index).arg(j+1));
        }
    }
    return tipsList;
}

QMap<QString, QString> KitMgr::getColorOption()
{
    if(!spoolColorMap.isEmpty())
        return spoolColorMap;
    QFile file(colorFilePath);
    if(file.open(QFile::ReadOnly))
    {
        QByteArray byteArray = file.readAll();
        QJsonDocument jsonDocu = QJsonDocument::fromJson(byteArray);
        if(jsonDocu.isObject())
        {
            QJsonObject obj_root = jsonDocu.object();
            QJsonObject objColor = obj_root["colors"].toObject();
            for(QString key:objColor.keys())
            {
                spoolColorMap[key] = objColor[key].toString();
            }
        }
    }
    return spoolColorMap;
}


QString KitMgr::getInverseFontColor(QString _color)
{
    QStringList colors = _color.split(",");
    QString fontColor = QString("255,255,255");
    int r = QString(colors[0]).toInt();
    int g = QString(colors[1]).toInt();
    int b = QString(colors[2]).toInt();
    double darkness = 1 - (0.299 * r + 0.587 * g + 0.114 *b) / 255;
    if (darkness < 0.5) {
        fontColor = QString("0,0,0");
    }
    return fontColor;
}
