#include "kitmgr.h"

KitMgr::KitMgr()
{
    normTypeStr << QString::fromLocal8Bit("无");
    normTypeStr << QString::fromLocal8Bit("普通");
    normTypeStr << QString::fromLocal8Bit("内质控");
    configPath = "E:/Project/Qt/KitEditor/data/kit/E015/MultiFluoInfo.json";
    filePath = "E:/Project/QT/KitEditor/data/kit/E015/";
    loadKitConfig();
}

void KitMgr::loadKitConfig()
{
    QFile file(configPath);
    if(file.open(QFile::ReadOnly))
    {
        QByteArray byteArray = file.readAll();
        kitConfig = getProcedureListByData(byteArray);
    }
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
