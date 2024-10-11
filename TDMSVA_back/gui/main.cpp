#include <QApplication>
#include <QDesktopWidget>

#include "massdbwidget.h"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  MassDBWidget w;

  // 固定窗口大小并居中
  //  w.setFixedSize(w.width(), w.height());
  w.move((QApplication::desktop()->width() - w.width()) / 2,
         (QApplication::desktop()->height() - w.height()) / 2);

  w.show();

  return a.exec();
}
