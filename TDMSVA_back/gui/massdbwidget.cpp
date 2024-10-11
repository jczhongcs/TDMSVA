#include "massdbwidget.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QToolTip>

#include "./ui_massdbwidget.h"
#include "file_str.h"
#include "msdata.h"
#include "prsm_reader.h"
#include "util.h"

MassDBWidget::MassDBWidget(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MassDBWidget) {
  ui->setupUi(this);
  init_configration();
}

MassDBWidget::~MassDBWidget() {
  delete m_massdb;
  delete m_standardItemModel;
  delete m_ContextMenu1;
  delete ui;
}

// 初始化配置
void MassDBWidget::init_configration() {
  // 如果还未打开数据库，则禁用其他的选项
  if (m_massdb == nullptr) {
    ui->new_package->setEnabled(false);
    ui->import_file->setEnabled(false);
  }

  // 初始化包的 treeView 配置
  m_standardItemModel = new QStandardItemModel(ui->package_table_view);
  m_standardItemModel->setHorizontalHeaderLabels(QStringList()
                                                 << "Species"
                                                 << "Tissue"
                                                 << "Package"
                                                 << "UUID");
  ui->package_table_view->setModel(m_standardItemModel);
  ui->package_table_view->setContextMenuPolicy(Qt::CustomContextMenu);         // 为 treeView 注册右键菜单
  ui->package_table_view->setSortingEnabled(true);                             // 设置 treeView 可以排序
  ui->package_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);  // tableView 禁止修改

  m_standardItemModel2 = new QStandardItemModel(ui->ms_file_tableView);
  m_standardItemModel2->setHorizontalHeaderLabels(QStringList() << "File");
  for (const auto& i : m_package_list) {
    //    auto* item1 = new QStandardItem("");
    auto* item = new QStandardItem(i);
    item->setCheckState(Qt::Unchecked);
    m_standardItemModel2->appendRow(item);
  }
  ui->ms_file_tableView->setModel(m_standardItemModel2);
  ui->ms_file_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->ms_file_tableView->verticalHeader()->setVisible(false);
  ui->ms_file_tableView->setShowGrid(false);
  ui->ms_file_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // tableView 禁止修改

  ui->table_key_list->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->table_key_list->setEditTriggers(QAbstractItemView::NoEditTriggers);

  ui->table_ms_data->setSortingEnabled(true);  // 设置 treeView 可以排序
  ui->table_ms_data->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

  if (m_massdb == nullptr || m_package == nullptr) {
    ui->ms_file_tableView->setEnabled(false);
  }
}

// 菜单栏上的命令
void MassDBWidget::on_close_triggered() {
  // 关闭窗口
  this->close();
}  // 关闭窗口
void MassDBWidget::on_new_db_triggered() {
  // 打开选择文件夹窗口
  m_massdb_path = QFileDialog::getExistingDirectory(this, tr("Build Database"),
                                                    "/",
                                                    QFileDialog::ShowDirsOnly |
                                                        QFileDialog::DontResolveSymlinks);
  if (!m_massdb_path.isEmpty()) {
    // leveldb 打开 path 路径的数据库
    m_massdb = new massdb::MassDB();

    leveldb::Status status = m_massdb->Init(m_massdb_path.toStdString(), true);

    if (!status.ok()) {
      // 弹出提示框，让用户选择是否创建数据库
      QMessageBox::information(this, tr("Warn"), tr("Failed to create a database\nThe folder may be occupied or there may be no permissions"));
    }
    // 获取 m_massdb_path 的最后一个文件夹名
    QStringList path_list = m_massdb_path.split("/");
    const QString& m_massdb_name = path_list.at(path_list.size() - 1);
    ui->label_table_name->setText(m_massdb_name);
    m_package_map = m_massdb->get_alis_to_uuid();
    ui->new_package->setEnabled(true);
  }
  loadPackageList();
}  // 新建数据库
void MassDBWidget::on_import_db_triggered() {
  // 打开选择文件夹窗口
  m_massdb_path = QFileDialog::getExistingDirectory(this, tr("Import Database"),
                                                    "/",
                                                    QFileDialog::ShowDirsOnly |
                                                        QFileDialog::DontResolveSymlinks);
  if (!m_massdb_path.isEmpty()) {
    // leveldb 打开 path 路径的数据库
    m_massdb = new massdb::MassDB();
    leveldb::Status status = m_massdb->Init(m_massdb_path.toStdString(), false);
    if (!status.ok()) {
      // 弹出提示框，让用户选择是否创建数据库
      QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, tr("Warn"), tr("The database does not exist, whether to create？"),
                                    QMessageBox::Yes | QMessageBox::No);
      if (reply == QMessageBox::Yes) {
        // 创建数据库
        status = m_massdb->Init(m_massdb_path.toStdString(), true);
        if (!status.ok()) {
          QMessageBox::information(this, tr("Warn"), tr("The database creation failed"));
          return;
        } else {
          QMessageBox::information(this, tr("Warn"), tr("The database is successfully created"));
        }
      } else {
        return;
      }
    }
    m_package_map = m_massdb->get_alis_to_uuid();
    // 加载数据库中的包
    ui->new_package->setEnabled(true);
    // 获取 m_massdb_path 的最后一个文件夹名
    QStringList path_list = m_massdb_path.split("/");
    const QString& m_massdb_name = path_list.at(path_list.size() - 1);
    ui->label_table_name->setText(m_massdb_name);
    ui->new_package->setEnabled(true);
    // 加载表列表
    loadPackageList();
  }
}  // 导入数据库
void MassDBWidget::on_new_package_triggered() {
  treeview_menu_new_package();
}  // 新建包

void MassDBWidget::on_import_rms_triggered() {
  if (m_massdb == nullptr) {
    QMessageBox::information(this, tr("Warn"), tr("Create or import the database first"));
  }
  if (m_package == nullptr) {
    QMessageBox::information(this, tr("Warn"), tr("Create the Package first"));
  }
  QString spfile = QFileDialog::getOpenFileName(
      this,
      "Select the mass spectrum file",
      "/",
      "Spectrum file(*.mzXML *.mzML *.mzxml *.mzml)");
  if (spfile.isEmpty()) {
    return;
  }
  m_package->PutMSFile(spfile.toStdString());
  m_standardItemModel2->item(0, 0)->setCheckState(Qt::Checked);
}  // 导入 ms 文件
void MassDBWidget::on_import_dms_triggered() {
  if (m_massdb == nullptr) {
    QMessageBox::information(this, tr("Warn"), tr("Create or import the database first"));
  }
  if (m_package == nullptr) {
    QMessageBox::information(this, tr("Warn"), tr("Create the Package first"));
  }
  QString spfile = QFileDialog::getOpenFileName(
      this,
      "导入解卷积后的文件",
      "/",
//      "解卷积后的文件(*ms2.msalign)");
      "解卷积后的文件(*.msalign)");
  if (spfile.isEmpty()) {
    return;
  }
  m_package->PutDeconvolutedMSFile(spfile.toStdString());
  m_standardItemModel2->item(1, 0)->setCheckState(Qt::Checked);
}  // 导入解卷积后的1级质谱文件
void MassDBWidget::on_import_ims_triggered() {
  if (m_massdb == nullptr) {
    QMessageBox::information(this, tr("Warn"), tr("Create or import the database first"));
  }
  if (m_package == nullptr) {
    QMessageBox::information(this, tr("Warn"), tr("Create the Package first"));
  }
  QString spfile = QFileDialog::getOpenFileName(
      this,
      "选择鉴定后的质谱文件",
      "/",
      "鉴定文件(*.csv *.tsv)");
  if (spfile.isEmpty()) {
    return;
  }
  m_package->PutIdentifiedMSFile(spfile.toStdString());
  m_standardItemModel2->item(2, 0)->setCheckState(Qt::Checked);
}  // 导入鉴定后文件
void MassDBWidget::on_import_dm1_triggered(){
    if (m_massdb == nullptr) {
        QMessageBox::information(this, tr("Warn"), tr("Create or import the database first"));
    }
    if (m_package == nullptr) {
        QMessageBox::information(this, tr("Warn"), tr("Create the Package first"));
    }
    QString spfile = QFileDialog::getOpenFileName(
            this,
            "导入解卷积后的1级质谱文件",
            "/",
//      "解卷积后的文件(*ms2.msalign)");
            "解卷积后的1级质谱文件(*ms1.msalign)");
    if (spfile.isEmpty()) {
        return;
    }
    m_package->PutDeconMS1File(spfile.toStdString());
    m_standardItemModel2->item(3, 0)->setCheckState(Qt::Checked);
}   // 导入鉴定后的2级质谱文件

void MassDBWidget::loadPackageList() {
  // 不能创建相同的报名
  for (auto& package : (*m_package_map)) {
    QList<QStandardItem*> items;
    auto* item1 = new QStandardItem(QString::fromStdString(m_massdb->package_info_[package.second][0]));
    auto* item2 = new QStandardItem(QString::fromStdString(m_massdb->package_info_[package.second][1]));
    auto* item3 = new QStandardItem(QString::fromStdString(package.first));
    auto* item4 = new QStandardItem(QString::fromStdString(package.second));
    items.append(item1);
    items.append(item2);
    items.append(item3);
    items.append(item4);
    m_standardItemModel->appendRow(items);
  }
  auto* proxyModel = new QSortFilterProxyModel(this);
  proxyModel->setSourceModel(m_standardItemModel);
  ui->package_table_view->setModel(proxyModel);
}  // 从数据库中加载包

// 包列表的右键菜单相关命令
void MassDBWidget::on_package_table_view_customContextMenuRequested(const QPoint& pos) {
  // 在 treeView 中添加右键菜单
  QModelIndex index = ui->package_table_view->indexAt(pos);
  if (index.isValid()) {
    m_ContextMenu1 = new QMenu(this);
    // SLOT 中的参数一个函数
    m_ContextMenu1->addAction("Select Package", this, SLOT(treeview_menu_open_package()));
    m_ContextMenu1->addAction("Delete Package", this, SLOT(treeview_menu_delete_package()));
    m_ContextMenu1->exec(QCursor::pos());
  } else {
    // 如果点击的是空白区域，则弹出新建包的菜单
    m_ContextMenu1 = new QMenu(this);
    m_ContextMenu1->addAction("Create Package", this, SLOT(treeview_menu_new_package()));
    m_ContextMenu1->exec(QCursor::pos());
  }
}
void MassDBWidget::treeview_menu_new_package() {
  // 弹出一个可输入的对话框，输入包名
  if (!m_massdb) {
    QMessageBox::information(this, tr("Warn"), tr("Create or import the database first"));
    return;
  }
  MyDialog* dialog = new MyDialog(this);
  if (dialog->exec() == QDialog::Accepted) {
    QString being = dialog->lineEdit1->text();
    QString tissue = dialog->lineEdit2->text();
    QString package_name = dialog->lineEdit3->text();
    m_package = m_massdb->OpenPackage(package_name.toStdString(), true);
    m_package->PutInfo(being.toStdString(), tissue.toStdString(), package_name.toStdString());
    // 添加到 treeView 中

    QList<QStandardItem*> items;
    items.append(new QStandardItem(being));
    items.append(new QStandardItem(tissue));
    items.append(new QStandardItem(package_name));
    items.append(new QStandardItem(QString::fromStdString(m_package->get_uuid())));
    m_standardItemModel->appendRow(items);
  }
  delete dialog;
}  // 新建包
void MassDBWidget::treeview_menu_open_package() {
  // 使用 proxy 代理后会改变当前 ui->package_table_view->currentIndex()；
  // 所以要使用相对位置来获取当前的 index
  QModelIndex current_index = ui->package_table_view->currentIndex();
  QModelIndex index = current_index;

//  loadPackageList();

  if (ui->package_table_view->model()) {
    auto* proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->package_table_view->model());
    index = proxyModel->mapToSource(current_index);
  }
  // 获取 item 的 UUID
  QString package_name = m_standardItemModel->item(index.row(), 2)->text();
  // 打开包
  m_package = m_massdb->OpenPackage(package_name.toStdString(), true);
  // 启用状态
  ui->ms_file_tableView->setEnabled(true);
  ui->import_file->setEnabled(true);

  if (m_package->ExistMSFile()) {
    // 反向解析 standItem
    m_standardItemModel2->item(0, 0)->setCheckState(Qt::Checked);
  } else {
    m_standardItemModel2->item(0, 0)->setCheckState(Qt::Unchecked);
  }
  if (m_package->ExistDeconvolutedMSFile()) {
    // 反向解析 standItem
    m_standardItemModel2->item(1, 0)->setCheckState(Qt::Checked);
  } else {
    m_standardItemModel2->item(1, 0)->setCheckState(Qt::Unchecked);
  }
  if (m_package->ExistIdentifiedMSFile()) {
    // 反向解析 standItem
    m_standardItemModel2->item(2, 0)->setCheckState(Qt::Checked);
  } else {
    m_standardItemModel2->item(2, 0)->setCheckState(Qt::Unchecked);
  }
  if (m_package->ExistDeconMS1File()) {
      // 反向解析 standItem
      m_standardItemModel2->item(3, 0)->setCheckState(Qt::Checked);
  } else {
      m_standardItemModel2->item(3, 0)->setCheckState(Qt::Unchecked);
  }
}  // 打开包
void MassDBWidget::treeview_menu_delete_package() {
  // 获取当前选中的 item
  QModelIndex index = ui->package_table_view->currentIndex();
  // 弹出一个确认对话框，确认是否删除
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, tr("Warn"), tr("Whether or not to delete the Package？"),
                                QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    // 获取 item 的 UUID
    std::string package_name = m_standardItemModel->item(index.row(), 2)->text().toStdString();
    // 删除包
    // 如果 m_package 打开包以后才会赋值，所以这里有空指针问题
    if (m_package) {
      if ((*m_package_map)[package_name] == m_package->get_uuid()) {
        delete m_package;
        m_package = nullptr;

        m_standardItemModel2->item(0, 0)->setCheckState(Qt::Unchecked);
        m_standardItemModel2->item(1, 0)->setCheckState(Qt::Unchecked);
        m_standardItemModel2->item(2, 0)->setCheckState(Qt::Unchecked);
        m_standardItemModel2->item(3, 0)->setCheckState(Qt::Unchecked);

        ui->ms_file_tableView->setEnabled(false);
        ui->import_file->setEnabled(false);
        ui->textBrowser->clear();

        delete m_standardItemModel3;
        m_standardItemModel3 = nullptr;
        delete m_standardItemModel4;
        m_standardItemModel4 = nullptr;
        --m_package_count;
      }
    }
    m_massdb->DeletePackage(package_name);
    // 移除 item
    m_standardItemModel->removeRow(index.row());
  }
}  // 删除包

// 质谱文件列表的右键菜单相关命令
void MassDBWidget::on_ms_file_tableView_customContextMenuRequested(const QPoint& pos) {
  // 在 tableView 中添加右键菜单
  QModelIndex index = ui->ms_file_tableView->indexAt(pos);
  if (index.isValid()) {
    m_ContextMenu2 = new QMenu(this);
    // SLOT 中的参数一个函数
    m_ContextMenu2->addAction("Open File", this, SLOT(ms_file_tableView_menu_open_file()));
    m_ContextMenu2->addAction("Import File", this, SLOT(ms_file_tableView_menu_import_file()));
    m_ContextMenu2->addAction("Delete File", this, SLOT(ms_file_tableView_menu_delete_file()));
    m_ContextMenu2->exec(QCursor::pos());
  }
}
void MassDBWidget::ms_file_tableView_menu_open_file() {
  int index = ui->ms_file_tableView->currentIndex().row();
  ui->textBrowser->clear();
  if (m_standardItemModel4 != nullptr) {
    m_standardItemModel4->clear();
  }
  if (m_standardItemModel3 != nullptr) {
    m_standardItemModel3->clear();
  }
  std::vector<std::string> key_list;
  switch (index) {
    case 0: {  // 打开 MS 文件
      key_list = m_package->GetMSFileKey();
      if (key_list.empty()) return;
      loadMSKey(key_list);
      file_type = 0;  // 在解析数据的时候用于判断是哪种类型的文件，TODO：应该有其他方法
      break;
    }
    case 1: {  // 打开 Deconvoluted MS 文件
      key_list = m_package->GetDeconvolutedMSFileKey();
      if (key_list.empty()) return;
      loadMSKey(key_list);
      file_type = 1;
      break;
    }
    case 2: {
      auto list = m_package->GetIdentifiedMSFileKeyByIndex();
      if (list.empty()) return;
      loadIdentifiedMSKey(list);
      file_type = 2;
      break;
    }
    case 3: {
      key_list = m_package->GetDeconMS1Key();
      if (key_list.empty()) return;
      loadMSKey(key_list);
      file_type = 3;
      break;
    }
  }
}  // 打开现有的质谱文件
void MassDBWidget::ms_file_tableView_menu_import_file() {
  int index = ui->ms_file_tableView->currentIndex().row();
  QString spfile;
  switch (index) {
    case 0: {  // 导入 MS 文件
      spfile = QFileDialog::getOpenFileName(
          this,
          "Select Deconvolution File",
          "/",
          "DeconvolutionFile(*.mzXML *.mzML *.mzxml *.mzml)");
      if (spfile.isEmpty()) {
        return;
      }
      m_package->PutMSFile(spfile.toStdString());
      m_standardItemModel2->item(0, 0)->setCheckState(Qt::Checked);
      break;
    }
    case 1: {  // 导入 Deconvoluted MS 文件
      spfile = QFileDialog::getOpenFileName(
          this,
          "Select Deconvoluted Ms2 File",
          "/",
//          "Ms2File(*ms2.msalign)");
          "Ms2File(*ms2.msalign)");
      if (spfile.isEmpty()) {
        return;
      }
      m_package->PutDeconvolutedMSFile(spfile.toStdString());
      m_standardItemModel2->item(1, 0)->setCheckState(Qt::Checked);
      break;
    }
    case 2: {
      // 导入 Identified MS 文件
      spfile = QFileDialog::getOpenFileName(
          this,
          "Select Prsm File",
          "/",
          "PrsmFile(*.csv *.tsv)");
      if (spfile.isEmpty()) {
        return;
      }
      m_package->PutIdentifiedMSFile(spfile.toStdString());
      m_standardItemModel2->item(2, 0)->setCheckState(Qt::Checked);
      break;
    }
    case 3: {
        spfile = QFileDialog::getOpenFileName(
                this,
                "Select Deconvoluted Ms1 File",
                "/",
//          "Ms2File(*ms2.msalign)");
                "DeconMs1File(*ms1.msalign)");
        if (spfile.isEmpty()) {
            return;
        }
        m_package->PutDeconMS1File(spfile.toStdString());
        m_standardItemModel2->item(3, 0)->setCheckState(Qt::Checked);
        break;
    }
    default:
      break;
  }
}  // 从本地导入相应的质谱文件数据到数据库
void MassDBWidget::ms_file_tableView_menu_delete_file() {
  int index = ui->ms_file_tableView->currentIndex().row();
  switch (index) {
    case 0: {  // 删除 MS 文件
      m_package->DeleteMSFile();
      m_standardItemModel2->item(0, 0)->setCheckState(Qt::Unchecked);
      break;
    }
    case 1: {  // 删除 Deconvoluted MS 文件
      m_package->DeleteDeconvolutedMSFile();
      m_standardItemModel2->item(1, 0)->setCheckState(Qt::Unchecked);
      break;
    }
    case 2: {  // 删除 Identified MS 文件
      m_package->DeleteIdentifiedMSFile();
      m_standardItemModel2->item(2, 0)->setCheckState(Qt::Unchecked);
      break;
    }
    case 3: {  // 删除 Deconvoluted MS 文件
      m_package->DeleteDeconMS1File();
      m_standardItemModel2->item(3, 0)->setCheckState(Qt::Unchecked);
      break;
    }
  }
}  // 删除质谱文件数据

// 键列表右键菜单相关命令
void MassDBWidget::on_table_key_list_customContextMenuRequested(const QPoint& pos) {
  // 在 tableView 中添加右键菜单
  QModelIndex index = ui->table_key_list->indexAt(pos);
  if (index.isValid()) {
    m_ContextMenu2 = new QMenu(this);
    // SLOT 中的参数一个函数
    m_ContextMenu2->addAction("Load Data", this, SLOT(ms_table_key_list_menu_load_file()));
    m_ContextMenu2->addAction("Delete Data", this, SLOT(ms_table_key_list_menu_delete_file()));
    m_ContextMenu2->exec(QCursor::pos());
  }
}
void MassDBWidget::ms_table_key_list_menu_load_file() {
  // 获取当前 table_key_list item 中的内容
  QModelIndex current_index = ui->table_key_list->currentIndex();
  auto* proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->table_key_list->model());
  QModelIndex index = proxyModel->mapToSource(current_index);
  QString key = m_standardItemModel3->item(index.row(), 0)->text();
  // 将 key 转换为整型
  int key_int = key.toInt();
  // 加载数据
  switch (file_type) {
    case 0: {
      loadMSData(key_int);
      break;
    }
    case 1: {
      loadDeconvMSData(key_int);
      break;
    }
    case 2: {
      loadIdentifiedMSData(key_int);
      break;
    }
    case 3: {
      loadDeconMS1Data(key_int);
      break;
    }
  }
}
void MassDBWidget::ms_table_key_list_menu_delete_file() {
  //   获取当前 table_key_list item 中的内容
  QModelIndex index = ui->table_key_list->currentIndex();
  QString key = m_standardItemModel3->item(index.row(), 0)->text();
  // 将 key 转换为整型
  int key_int = key.toInt();
  // 删除数据
  switch (file_type) {
    case 0: {
      m_package->DeleteOneMSItem(key_int);
      break;
    }
    case 1: {
      m_package->DeleteOneDeconvolutedMSItem(key_int);
      break;
    }
    case 2: {
      m_package->DeleteOneIdentifiedMSItem(key_int);
      break;
    }
    case 3: {
        m_package->DeleteOneDeconMS1Item(key_int);
        break;
    }
  }
  // 移除元素
  m_standardItemModel3->removeRow(index.row());
}

// 加载相应的质谱数据
void MassDBWidget::loadMSKey(std::vector<std::string>& key_list) {
  m_standardItemModel3 = new QStandardItemModel(ui->table_key_list);
  m_standardItemModel3->setHorizontalHeaderLabels(QStringList()
                                                  << "ID");
  for (const auto& i : key_list) {
    QList<QStandardItem*> items;
    items.append(new QStandardItem(QString::fromStdString(i)));
    m_standardItemModel3->appendRow(items);
  }

  // 过滤模型
  auto* proxyModel = new QSortFilterProxyModel(this);
  proxyModel->setSourceModel(m_standardItemModel3);
  ui->table_key_list->setModel(proxyModel);
}  // 加载 MS 数据
void MassDBWidget::loadMSData(int id) {
  toppic::RawMsPtr ms = m_package->GetMSFileSingleData(id);
  QString header = QString::fromStdString(massdb::Package::parseHeaderToString(ms->getMsHeaderPtr()));
  // 头部信息输出到文本框中
  //  std::cout << header.toStdString() << std::endl;
  ui->textBrowser->setText(header);
  m_standardItemModel4 = new QStandardItemModel(ui->table_ms_data);
  m_standardItemModel4->setHorizontalHeaderLabels(QStringList()
                                                  << "M/Z"
                                                  << "Itensity");
  for (int i = 0; i < ms->getPeakPtrVec().size(); i++) {
    QList<QStandardItem*> items;
    QStandardItem* item = new QStandardItem();
    item->setData(ms->getPeakPtr(i)->getPosition(), Qt::DisplayRole);
    items.append(new QStandardItem(QString::number(ms->getPeakPtr(i)->getPosition())));
    items.append(new QStandardItem(QString::number(ms->getPeakPtr(i)->getIntensity())));
    m_standardItemModel4->appendRow(items);
  }
  ui->table_ms_data->setModel(m_standardItemModel4);

}  // 将 MS 的头部数据输出到 textBrowser 中
void MassDBWidget::loadDeconvMSData(int id) {
  toppic::DeconvMsPtr ms = m_package->GetDeconvolutedMSFileSingleData(id);
  QString header = QString::fromStdString(massdb::Package::parseHeaderToString(ms->getMsHeaderPtr()));
  ui->textBrowser->setText(header);
  m_standardItemModel4 = new QStandardItemModel(ui->table_ms_data);
  m_standardItemModel4->setHorizontalHeaderLabels(QStringList()
                                                  << "Mass"
                                                  << "Itensity"
                                                  << "Charge");
  for (int i = 0; i < ms->getPeakPtrVec().size(); i++) {
    QList<QStandardItem*> items;
    items.append(new QStandardItem(QString::number(ms->getPeakPtr(i)->getPosition())));
    items.append(new QStandardItem(QString::number(ms->getPeakPtr(i)->getIntensity())));
    items.append(new QStandardItem(QString::number(ms->getPeakPtr(i)->getCharge())));
    m_standardItemModel4->appendRow(items);
  }

  ui->table_ms_data->setModel(m_standardItemModel4);
}
void MassDBWidget::loadDeconMS1Data(int id) {
    toppic::DeconvMsPtr ms = m_package->GetDeconMS1FileSingleData(id);
    QString header = QString::fromStdString(massdb::Package::parseHeaderToString(ms->getMsHeaderPtr()));
    ui->textBrowser->setText(header);
    m_standardItemModel4 = new QStandardItemModel(ui->table_ms_data);
    m_standardItemModel4->setHorizontalHeaderLabels(QStringList()
                                                            << "Mass"
                                                            << "Itensity"
                                                            << "Charge");
    for (int i = 0; i < ms->getPeakPtrVec().size(); i++) {
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QString::number(ms->getPeakPtr(i)->getPosition())));
        items.append(new QStandardItem(QString::number(ms->getPeakPtr(i)->getIntensity())));
        items.append(new QStandardItem(QString::number(ms->getPeakPtr(i)->getCharge())));
        m_standardItemModel4->appendRow(items);
    }

    ui->table_ms_data->setModel(m_standardItemModel4);
}


// 加载 IdentifiedMS 数据
void MassDBWidget::loadIdentifiedMSKey(vector<vector<string>>& list) {
  m_standardItemModel3 = new QStandardItemModel(ui->table_key_list);
  m_standardItemModel3->setHorizontalHeaderLabels(QStringList()
                                                  << "ID"
                                                  << "Protein Accession");
  for (const auto& i : list) {
    QList<QStandardItem*> items;
    items.append(new QStandardItem(QString::fromStdString(i[0])));
    items.append(new QStandardItem(QString::fromStdString(i[1])));
    m_standardItemModel3->appendRow(items);
  }

  // 过滤模型
  auto* proxyModel = new QSortFilterProxyModel(this);
  proxyModel->setSourceModel(m_standardItemModel3);
  ui->table_key_list->setModel(proxyModel);
  ui->table_key_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
}
void MassDBWidget::loadIdentifiedMSData(std::vector<std::vector<std::string>>& iden_file) {
  m_standardItemModel4 = new QStandardItemModel(ui->table_ms_data);
  // 列
  m_standardItemModel4->setColumnCount(iden_file[0].size());
  // 行
  m_standardItemModel4->setRowCount(iden_file.size());
  for (int i = 0; i < iden_file[0].size(); i++) {
    m_standardItemModel4->setHeaderData(i, Qt::Horizontal, tsv_file_head[i]);
  }
  for (int i = 0; i < iden_file.size(); i++) {
    for (int j = 0; j < iden_file[i].size(); j++) {
      m_standardItemModel4->setItem(i, j, new QStandardItem(QString::fromStdString(iden_file[i][j])));
    }
  }

  ui->table_ms_data->setModel(m_standardItemModel4);
}
void MassDBWidget::loadIdentifiedMSData(int id) {
  std::vector<std::string> ms = massdb::PrsmReader::string_to_prsm(m_package->GetIdentifiedMSFileSingleData(id));
  m_standardItemModel4 = new QStandardItemModel(ui->table_ms_data);
  m_standardItemModel4->setHorizontalHeaderLabels(QStringList()
                                                  << "Attribute"
                                                  << "Value");
  for (int i = 0; i < ms.size(); i++) {
    QList<QStandardItem*> items;
    items.append(new QStandardItem(tsv_file_head[i]));
    items.append(new QStandardItem(QString::fromStdString(ms[i])));
    m_standardItemModel4->appendRow(items);
  }
  ui->table_ms_data->setModel(m_standardItemModel4);
  ui->table_ms_data->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
}

// 按钮相关命令

void MassDBWidget::on_botton_get_key_clicked() {
  // 弹出提示框
  if (m_massdb == nullptr) {
    QToolTip::showText(QCursor::pos(), "Create or import the database first", this);
    return;
  }
  QString input = ui->input_key->text().trimmed();
  // 根据 + 符号分隔 key
  QStringList key_list = input.split("+");

  // key 格式： package + type + id
  string key = "package_list_01_";
  if (key_list.size() == 3) {
    // 判断 package 是否存在
    if ((*m_package_map).count(key_list[0].toStdString()) == 0) {
      QToolTip::showText(QCursor::pos(), "This Package does not exist", this);
      return;
    } else {
      key += (*m_package_map)[key_list[0].toStdString()];
    }
    // 判断类型是否存在
    if (ms_file_type.count(key_list[1]) == 0) {
      QToolTip::showText(QCursor::pos(), "This Package type not exist", this);
      return;
    } else {
      key += ms_file_type[key_list[1]] + "\1" + "\2";
    }
    key += massdb::Util::val_str(key_list[2].toInt());
    string value;
    leveldb::Status status = m_massdb->Get(leveldb::ReadOptions(), key, &value);
    if (status.ok()) {
      string type = ms_file_type[key_list[1]];
      switch (type[0]) {
        case 1: {
          // TODO 重构代码
          QString header = QString::fromStdString(massdb::Package::parseHeaderToString(massdb::MsData::string_to_header(value)));
          auto peak_list = massdb::MsData::string_to_peak_list(value);
          ui->textBrowser->setText(header);
          m_standardItemModel4 = new QStandardItemModel(ui->table_ms_data);
          m_standardItemModel4->setHorizontalHeaderLabels(QStringList()
                                                          << "M/Z"
                                                          << "Itensity");
          for (auto& i : peak_list) {
            QList<QStandardItem*> items;
            items.append(new QStandardItem(QString::number(i->getPosition())));
            items.append(new QStandardItem(QString::number(i->getIntensity())));
            m_standardItemModel4->appendRow(items);
          }
          ui->table_ms_data->setModel(m_standardItemModel4);
          break;
        }
        case 2: {
          QString header = QString::fromStdString(massdb::Package::parseHeaderToString(massdb::MsData::string_to_header(value)));
          auto peak_list = massdb::MsData::string_to_deconv_ms_peak_list(value);

          ui->textBrowser->setText(header);
          m_standardItemModel4 = new QStandardItemModel(ui->table_ms_data);
          m_standardItemModel4->setHorizontalHeaderLabels(QStringList()
                                                          << "Mass"
                                                          << "Itensity"
                                                          << "Charge");
          for (auto& i : peak_list) {
            QList<QStandardItem*> items;
            items.append(new QStandardItem(QString::number(i->getPosition())));
            items.append(new QStandardItem(QString::number(i->getIntensity())));
            items.append(new QStandardItem(QString::number(i->getCharge())));
            m_standardItemModel4->appendRow(items);
          }
          ui->table_ms_data->setModel(m_standardItemModel4);
          break;
        }
        case 3:
          auto iden_file = massdb::PrsmReader::string_to_prsm(value);
          m_standardItemModel4 = new QStandardItemModel(ui->table_ms_data);
          m_standardItemModel4->setColumnCount(iden_file.size());
          m_standardItemModel4->setRowCount(1);
          // QStringList stringList;
          // 加载头部
          for (int i = 0; i < iden_file.size(); i++) {
            m_standardItemModel4->setHeaderData(i, Qt::Horizontal, tsv_file_head[i]);
            m_standardItemModel4->setItem(0, i, new QStandardItem(QString::fromStdString(iden_file[i])));
          }
          ui->table_ms_data->setModel(m_standardItemModel4);
          break;
      }
    } else {
      QToolTip::showText(QCursor::pos(), "The key-value does not exist", this);
    }
  } else {
    QToolTip::showText(QCursor::pos(), "The key-value does not exist", this);
  }
  //    leveldb::Status status = m_package.
}  // 解析键值按钮
void MassDBWidget::on_pushButton_key_list_filter_clicked() {
  // 如果输入栏为空，清除当前过滤
  QModelIndex index = ui->table_key_list->currentIndex();
  auto* proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->table_key_list->model());
  if (proxyModel) {
    if (index.isValid()) {
      int row = index.column();
      proxyModel->setFilterRegExp(QRegExp(ui->lineEdit_key_list_filter->text(), Qt::CaseInsensitive, QRegExp::FixedString));
      proxyModel->setFilterKeyColumn(row);
    } else {
      proxyModel->setFilterRegExp("");
    }
  }
}  // 对键进行过滤
void MassDBWidget::on_pushButton_package_filter_clicked() {
  // 如果输入栏为空，清除当前过滤
  QModelIndex index = ui->package_table_view->currentIndex();
  QStringList str = ui->lineEdit_package_filter->text().split('|');

  if (str.size() == 2) {
    // 保存原始数据模型
    auto originalModel = ui->package_table_view->model();

    // 创建新的代理模型
    auto proxyModel = new CustomProxyModel;
    proxyModel->setFilterCondition(QRegExp(str[0], Qt::CaseInsensitive, QRegExp::FixedString), QRegExp(str[1], Qt::CaseInsensitive, QRegExp::FixedString));
    proxyModel->setSourceModel(m_standardItemModel);
    ui->package_table_view->setModel(proxyModel);

    // 释放原始数据模型
    delete originalModel;
  } else if (str.size() == 1) {
    auto* proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->package_table_view->model());
    if (proxyModel) {
      // 如果存在代理模型，先将其设置为原始模型
      ui->package_table_view->setModel(proxyModel->sourceModel());
    }
    // 检查 currentIndex 是否有效
    if (index.isValid()) {
      int row = index.column();
      proxyModel->setFilterRegExp(QRegExp(ui->lineEdit_package_filter->text(), Qt::CaseInsensitive, QRegExp::FixedString));
      proxyModel->setFilterKeyColumn(row);
    }
  } else {
    // 如果存在代理模型，先将其设置为原始模型
    auto* proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->package_table_view->model());
    if (proxyModel) {
      ui->package_table_view->setModel(proxyModel->sourceModel());
    } else {
      ui->package_table_view->setModel(m_standardItemModel);
    }
  }

  //  if (str.size() == 2) {
  //    auto proxyModel = new CustomProxyModel;
  //    auto originanModal = ui->table_ms_data->model();
  //    proxyModel->setFilterCondition(QRegExp(str[0], Qt::CaseInsensitive, QRegExp::FixedString), QRegExp(str[1], Qt::CaseInsensitive, QRegExp::FixedString));
  //    proxyModel->setSourceModel(m_standardItemModel);
  //    ui->package_table_view->setModel(proxyModel);
  //    //    proxyModel->setFilterRegExp(QRegExp());
  //  } else if (str.size() == 1) {
  //    //    ui->package_table_view->setModel(m_standardItemModel);
  //    auto* proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->package_table_view->model());
  //    int row = index.column();
  //    proxyModel->setFilterRegExp(QRegExp(ui->lineEdit_package_filter->text(), Qt::CaseInsensitive, QRegExp::FixedString));
  //    proxyModel->setFilterKeyColumn(row);
  //  } else {
  //        ui->package_table_view->setModel(m_standardItemModel);
  //  }

}  // 对包进行过滤

// 自定义输入框
MyDialog::MyDialog(QWidget* parent) : QDialog(parent) {
  lineEdit1 = new QLineEdit;
  lineEdit2 = new QLineEdit;
  lineEdit3 = new QLineEdit;
  okButton = new QPushButton("OK");
  cancelButton = new QPushButton("Cancel");

  QHBoxLayout* hLayout1 = new QHBoxLayout;
  hLayout1->addWidget(new QLabel("Species:"));
  hLayout1->addWidget(lineEdit1);

  QHBoxLayout* hLayout2 = new QHBoxLayout;
  hLayout2->addWidget(new QLabel("Tissue:"));
  hLayout2->addWidget(lineEdit2);

  QHBoxLayout* hLayout3 = new QHBoxLayout;
  hLayout3->addWidget(new QLabel("Package:"));
  hLayout3->addWidget(lineEdit3);

  QHBoxLayout* hLayout4 = new QHBoxLayout;
  hLayout4->addWidget(okButton);
  hLayout4->addWidget(cancelButton);

  QVBoxLayout* vLayout = new QVBoxLayout;
  vLayout->addLayout(hLayout1);
  vLayout->addLayout(hLayout2);
  vLayout->addLayout(hLayout3);
  vLayout->addLayout(hLayout4);

  setLayout(vLayout);

  connect(okButton, &QPushButton::clicked, this, &MyDialog::on_okButton_clicked);
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

  setWindowTitle("Input Information");
}
void MyDialog::on_okButton_clicked() {
  if (lineEdit1->text().isEmpty() || lineEdit2->text().isEmpty() || lineEdit3->text().isEmpty()) {
    QMessageBox::information(this, tr("Warn"), tr("Lack of information"));
  } else {
    accept();
  }
}

void CustomProxyModel::setFilterCondition(const QRegExp& condition1, const QRegExp& condition2) {
  m_filterRegExp1 = condition1;
  m_filterRegExp2 = condition2;
}
bool CustomProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
  QModelIndex index1 = sourceModel()->index(source_row, 0, source_parent);
  QModelIndex index2 = sourceModel()->index(source_row, 1, source_parent);
  QString data1 = sourceModel()->data(index1).toString();
  QString data2 = sourceModel()->data(index2).toString();
  bool match1 = (m_filterRegExp1.indexIn(data1) != -1);
  bool match2 = (m_filterRegExp2.indexIn(data2) != -1);
  return match1 && match2;
  //  return 1;
}