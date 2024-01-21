﻿#ifndef KITMODEL_H
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

  void copy(SpoolModel& target)
  {
      abbrName = target.abbrName;
      fullName = target.fullName;
      ctMin = target.ctMin;
      ctMax = target.ctMax;
      datumMin = target.datumMin;
      datumMax = target.datumMax;
      threshold = target.threshold;
      curveColor = target.curveColor;
      specimenNo = target.specimenNo;
      specimenType = target.specimenType;
      poolIndex = target.poolIndex;
      aisle = target.aisle;
  }
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

    void copy(KitModel& target)
    {
        abbrName = target.abbrName;
        barcode = target.barcode;
        dosage = target.dosage;
        filetype = target.filetype;
        fullName = target.fullName;
        distillFile = target.distillFile;
        ampFile = target.ampFile;
        for(int i = 0;i < target.spoolList.length();i++)
        {
            target.spoolList[i].copy(target.spoolList[i]);
            target.positiveSpoolList[i].copy(target.positiveSpoolList[i]);
            target.negativeSpoolList[i].copy(target.negativeSpoolList[i]);
        }
    }
};
