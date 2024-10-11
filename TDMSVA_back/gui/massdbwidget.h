
#ifndef MASSDBWIDGET_H
#define MASSDBWIDGET_H

#include <QDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QVBoxLayout>
#include <vector>

#include "leveldb/db.h"
#include "massdb.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MassDBWidget;
}
QT_END_NAMESPACE

class MassDBWidget : public QMainWindow {
  Q_OBJECT
public:
  MassDBWidget(QWidget* parent = nullptr);
  ~MassDBWidget();

private slots:
  void on_new_db_triggered();       // 新建数据库
  void on_new_package_triggered();  // 新建包
  void on_import_db_triggered();    // 导入数据库
  void on_close_triggered();        // 关闭文件

  void on_import_rms_triggered();  // 导入 ms 数据
  void on_import_dms_triggered();  // 导入解卷积后的文件
  void on_import_ims_triggered();  // 导入鉴定后的文件
  void on_import_dm1_triggered();  // 导入解卷积的1级质谱

  void on_package_table_view_customContextMenuRequested(const QPoint& pos);  // 响应 treeView 右键菜单
  void treeview_menu_new_package();                                          // treeView 右键菜单中的新建包
  void treeview_menu_delete_package();                                       // treeView 右键菜单中的删除包
  void treeview_menu_open_package();                                         // treeView 右键菜单中的打开包

  void on_ms_file_tableView_customContextMenuRequested(const QPoint& pos);  // 响应 ms_file 右键菜单
  void ms_file_tableView_menu_open_file();                                  // 打开现有的文件
  void ms_file_tableView_menu_import_file();                                // 从外部文件导入
  void ms_file_tableView_menu_delete_file();

  void on_table_key_list_customContextMenuRequested(const QPoint& pos);  // 相应 table_key_list 右键菜单
  void ms_table_key_list_menu_load_file();
  void ms_table_key_list_menu_delete_file();

  void on_botton_get_key_clicked();
  void on_pushButton_key_list_filter_clicked();
  void on_pushButton_package_filter_clicked();

private:
  // 初始化包列表
  void loadPackageList();
  void init_configration();

  int file_type = 0;

  void loadMSKey(std::vector<std::string>& ms_key);
  void loadMSData(int id);

  void loadDeconvMSData(int id);
  void loadDeconMS1Data(int id);


  void loadIdentifiedMSKey(vector<vector<string>>& list);
  void loadIdentifiedMSData(std::vector<std::vector<std::string>>& iden_file);
  void loadIdentifiedMSData(int id);

  Ui::MassDBWidget* ui;

  QStandardItemModel* m_standardItemModel = nullptr;   // 用于 treeView，展示包
  QStandardItemModel* m_standardItemModel2 = nullptr;  // 用于 tableView，展示质谱文件
  QStandardItemModel* m_standardItemModel3 = nullptr;  // 用于 tableView，展示键
  QStandardItemModel* m_standardItemModel4 = nullptr;  // 用于 tableView，展示值

  QMenu* m_ContextMenu1 = nullptr;  // treeView 右键菜单
  QMenu* m_ContextMenu2 = nullptr;  // treeView 右键菜单
  QMenu* m_ContextMenu3 = nullptr;  // treeView 右键菜单
  QMenu* m_ContextMenu4 = nullptr;  // treeView 右键菜单

  massdb::MassDB* m_massdb = nullptr;
  massdb::Package* m_package = nullptr;

  QString m_massdb_path;                                        // 包路径
  size_t m_package_count = 0;                                   // 记录包的数量
  std::map<std::string, std::string>* m_package_map = nullptr;  // 包列表映射
};

class MyDialog : public QDialog {
  Q_OBJECT

private slots:
  void on_okButton_clicked();

public:
  MyDialog(QWidget* parent = nullptr);
  QLineEdit* lineEdit1;
  QLineEdit* lineEdit2;
  QLineEdit* lineEdit3;
  QPushButton* okButton;
  QPushButton* cancelButton;
};

class CustomProxyModel : public QSortFilterProxyModel {
public:
  CustomProxyModel(QObject* parent = nullptr) : QSortFilterProxyModel(parent) {}

  void setFilterCondition(const QRegExp& condition1, const QRegExp& condition2);

protected:
  bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private:
  QRegExp m_filterRegExp1;
  QRegExp m_filterRegExp2;
};

#endif  // MASSDBWIDGET_H
