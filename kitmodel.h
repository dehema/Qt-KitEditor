#ifndef KITMODEL_H
#define KITMODEL_H

#endif // KITMODEL_H

#include <QString>
#include <QList>

struct SpoolModel {
  //简称/内部缩写
  QString abbrName;
  //全称/全部名称
  QString fullName;
  //ct下限
  int ctMin;
  //ct上限
  int ctMax;
  //基线下限
  int datumMin;
  //基线上限
  int datumMax;
  //阈值
  int threshold;
  //曲线颜色
  QString curveColor;
  //指标ID
  int specimenNo;
  //指标类型
  int specimenType;
  //索引
  int poolIndex;
  //通道
  QString aisle;
};

struct KitModel
{
public:
    //表名 体系简称
    QString abbrName = QString("");
    //条码编号
    QString barcode = QString("");
    //规格
    int dosage = 0;
    //文件类型
    QString filetype = QString("");
    //全称（中文名）
    QString fullName = QString("");
    //先忽略
    QString distillFile = QString("");
    //温控协议 扩增流程文件名称
    QString ampFile = QString("");
    //通道
    QList<SpoolModel> spoolList;
    QList<SpoolModel> positiveSpoolList;
    QList<SpoolModel> negativeSpoolList;

    KitModel()
    {
        spoolList = QList<SpoolModel>();
        positiveSpoolList = QList<SpoolModel>();
        negativeSpoolList = QList<SpoolModel>();
    }
};
