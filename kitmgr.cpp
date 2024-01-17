#include "kitmgr.h"

KitMgr::KitMgr()
{
    normTypeStr = {"无", "普通", "内质控"};
    configPath = "E:/Project/QT/KitEditor/data/Default.json";
    filePath = "E:/Project/QT/KitEditor/data/kit/E015/";
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
    return list;
}

void KitMgr::sortKitBySpecimenNo(KitModel _kitModel)
{
    sortSpoolBySpecimenNo(_kitModel.spoolList);
    sortSpoolBySpecimenNo(_kitModel.positiveSpoolList);
    sortSpoolBySpecimenNo(_kitModel.negativeSpoolList);
}

void KitMgr::sortSpoolBySpecimenNo(QList<SpoolModel> _spoolList)
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

void KitMgr::sortKitByPoolIndex(KitModel _kitModel)
{
    sortSpoolByPoolIndex(_kitModel.spoolList);
    sortSpoolByPoolIndex(_kitModel.positiveSpoolList);
    sortSpoolByPoolIndex(_kitModel.negativeSpoolList);
}

void KitMgr::sortSpoolByPoolIndex(QList<SpoolModel> _spoolList)
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
