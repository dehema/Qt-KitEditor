#include "kitmgr.h"

KitMgr::KitMgr()
{
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
        spool.abbrName = obj.value("abbrName").toString();
        spool.fullName = obj.value("fullName").toString();
        spool.ctMin = obj.value("ctMin").toInt();
        spool.ctMax = obj.value("ctMax").toInt();
        spool.datumMin = obj.value("datumMin").toInt();
        spool.datumMax = obj.value("datumMin").toInt();
        spool.threshold = obj.value("threshold").toInt();
        spool.curveColor = obj.value("curveColor").toString();
        spool.specimenNo = obj.value("specimenNo").toInt();
        spool.specimenType = obj.value("specimenType").toInt();
        spool.poolIndex = obj.value("poolIndex").toInt();
        spool.aisle = obj.value("aisle").toString();
        list.append(spool);
    }
    return list;
}
