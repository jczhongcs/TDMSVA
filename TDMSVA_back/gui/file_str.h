//
// Created by Xsakura on 2023/4/29.
//

#ifndef MASSDB_FILE_STR_H
#define MASSDB_FILE_STR_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QVector<QString> m_package_list = {
    "RawFile",
    "MsalignFile",
    "PrsmFile",
    "DeconMs1File"};  // 包列表

QVector<QString> tsv_file_head = {
    "Data file name",
    "Prsm ID",
    "Spectrum ID",
    "Fragmentation",
    "Scan(s)",
    "Retention time",
    "#peaks",
    "Charge",
    "Precursor mass",
    "Adjusted precursor mass",
    "Proteoform ID",
    "Feature intensity",
    "Feature score",
    "Feature apex time",
    "#Protein hits",
    "Protein accession",
    "Protein description",
    "First residue",
    "Last residue",
    "Special amino acids",
    "Database protein sequence",
    "Proteoform",
    "Proteoform mass",
    "Protein N-terminal form",
    "Fixed PTMs",
    "#unexpected modifications",
    "unexpected modifications",
    "#variable PTMs",
    "variable PTMs",
    "MIScore",
    "#matched peaks",
    "#matched fragment ions",
    "E-value",
    "Spectrum-level Q-value",
    "Proteoform-level Q-value"};

// 用于保存质谱文件类型的map
std::unordered_map<QString, std::string> ms_file_type = {
    {"rms", "\1"},
    {"dms", "\2"},
    {"ims", "\3"},
    {"dm1", "\4"},
    {"1", "\1"},
    {"2", "\2"},
    {"3", "\3"},
    {"4", "\4"},
    {"RawFile", "\1"},
    {"DeconvolutionFile", "\2"},
    {"PrsmFile", "\3"},
    {"DecM1File", "\4"},
};

#endif  // MASSDB_FILE_STR_H
