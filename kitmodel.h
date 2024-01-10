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
  //指标
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
    QString abbrName;
    //条码编号
    QString barcode;
    //规格
    int dosage;
    //文件类型
    QString filetype;
    //全称（中文名）
    QString fullName;
    //先忽略
    QString distillFile;
    //温控协议 扩增流程文件名称
    QString ampFile;
    //通道
    QList<SpoolModel> spoolList;
    QList<SpoolModel> positiveSpoolList;
    QList<SpoolModel> negativeSpoolList;
};
